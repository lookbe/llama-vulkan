#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

#include "llama.h"
#include <iostream>
#include <sstream> // Required for std::ostringstream
#include <string>
#include <vector>

std::vector<llama_token> llama_common_tokenize(const struct llama_vocab *vocab,
                                               const std::string &text,
                                               bool add_special,
                                               bool parse_special = false);
std::string llama_common_token_to_piece(const struct llama_vocab *vocab,
                                        llama_token token, bool special = true);

struct llama_data {
  llama_model *model = NULL;
  llama_context *ctx = NULL;
  llama_sampler *smpl = NULL;

  void destroy() {
    if (smpl) {
      llama_sampler_free(smpl);
      smpl = NULL;
    }
    if (ctx) {
      llama_free(ctx);
      ctx = NULL;
    }

    if (model) {
      llama_model_free(model);
      model = NULL;
    }
  }

  int init() {
    llama_model_params model_params = llama_model_default_params();

    model = llama_model_load_from_file(
        "C:\\Users\\PC\\Downloads\\LFM2-2.6B-Q4_K_M.gguf", model_params);

    if (model == NULL) {
      return 1;
    }

    llama_context_params ctx_params = llama_context_default_params();
    ctx = llama_init_from_model(model, ctx_params);

    if (ctx == NULL) {
      llama_model_free(model);
      model == NULL;
      return 1;
    }

    llama_sampler_chain_params sparams = llama_sampler_chain_default_params();
    smpl = llama_sampler_chain_init(sparams);

    llama_sampler_chain_add(smpl, llama_sampler_init_top_k(40));
    llama_sampler_chain_add(smpl, llama_sampler_init_top_p(0.95f, 0));
    llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.80f));
    llama_sampler_chain_add(smpl, llama_sampler_init_dist(0xFFFFFFFF));

    return 0;
  }

  int generate(const std::string &prompt, std::string &response) {
    printf("%s\n", prompt.c_str());
    const llama_vocab *vocab = llama_model_get_vocab(model);

    std::vector<llama_token> tokens;
    tokens = llama_common_tokenize(vocab, prompt, true, false);

    // prepare a batch for the prompt
    llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());
    llama_token new_token_id;
    while (true) {
      const int n_ctx = llama_n_ctx(ctx);
      const int n_ctx_used = llama_memory_seq_pos_max(llama_get_memory(ctx), 0);

      if (n_ctx_used + batch.n_tokens > n_ctx) {
        return 1;
      }

      if (llama_decode(ctx, batch)) {
        return 1;
      }

      // sample the next token, check is it an end of generation?
      new_token_id = llama_sampler_sample(smpl, ctx, -1);
      if (llama_vocab_is_eog(vocab, new_token_id)) {
        printf("\n");
        break;
      }

      std::string piece =
          llama_common_token_to_piece(vocab, new_token_id, false);
      response += piece;
      printf("%s", piece.c_str());

      batch = llama_batch_get_one(&new_token_id, 1);
    }

    return 0;
  }
};

std::string create_lfm2_prompt_with_system(const std::string &user_message,
                               const std::string &system_message = "") {
  // Define the fixed components of the LFM2 chat template
  const std::string BOS_TOKEN = "<|startoftext|>";
  const std::string SYSTEM_START = "<|im_start|>system\n";
  const std::string MESSAGE_END = "\n<|im_end|>\n";
  const std::string USER_START = "<|im_start|>user\n";
  const std::string ASSISTANT_START = "<|im_start|>assistant\n";

  std::string prompt = BOS_TOKEN;

  // 1. Add the System Message if it is provided
  if (!system_message.empty()) {
    prompt += SYSTEM_START;
    prompt += system_message;
    prompt += MESSAGE_END;
  }

  // 2. Add the User Message
  prompt += USER_START;
  prompt += user_message;
  prompt += MESSAGE_END;

  // 3. Add the Assistant Start token to prompt for generation
  prompt += ASSISTANT_START;

  return prompt;
}

int main() {
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  llama_backend_init();

  llama_data data;
  if (data.init() != 0) {
    data.destroy();
    llama_backend_free();
    return 1;
  }

  std::string response;

  std::string sys_msg = "You are a concise assistant, reply as short as possible, maximum two sentence";

  std::string user_msg_1 = "Why is the sky blue?";
  if (data.generate(create_lfm2_prompt_with_system(user_msg_1, sys_msg), response) !=
      0) {
    data.destroy();
    llama_backend_free();
    return 1;
  }

  std::string user_msg_2 = "What is that scientific concept named after?";
  if (data.generate(create_lfm2_prompt_with_system(user_msg_2), response) !=
      0) {
    data.destroy();
    llama_backend_free();
    return 1;
  }

  data.destroy();
  llama_backend_free();

  return 0;
}
