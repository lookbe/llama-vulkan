#include "llama.h"
#include "ggml.h"
#include "ggml-alloc.h"
#include "ggml-backend.h"
#include "ggml-cpu.h"
#include "ggml-opt.h"
#include "common.h"
#include "chat.h"

#define COMMON_API __declspec(dllexport)

COMMON_API void llama_common_batch_clear(struct llama_batch &batch) {
  common_batch_clear(batch);
}

COMMON_API void llama_common_batch_add(struct llama_batch &batch, llama_token id,
                            llama_pos pos,
                            const std::vector<llama_seq_id> &seq_ids,
                            bool logits) {
  common_batch_add(batch, id, pos, seq_ids, logits);
}

COMMON_API std::vector<llama_token> llama_common_tokenize(const struct llama_context *ctx,
                                               const std::string &text,
                                               bool add_special,
                                               bool parse_special = false) {

  return common_tokenize(ctx, text, add_special, parse_special);
}

COMMON_API std::vector<llama_token> llama_common_tokenize(const struct llama_vocab *vocab,
                                               const std::string &text,
                                               bool add_special,
                                               bool parse_special = false) {
  return common_tokenize(vocab, text, add_special, parse_special);
}

COMMON_API std::string llama_common_token_to_piece(const struct llama_context *ctx,
                                        llama_token token,
                                        bool special = true) {
  return common_token_to_piece(ctx, token, special);
}

COMMON_API std::string llama_common_token_to_piece(const struct llama_vocab *vocab,
                                        llama_token token,
                                        bool special = true) {
  return common_token_to_piece(vocab, token, special);
}
