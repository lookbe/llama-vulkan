# generate_exports_def.py
import re, sys, os

header = sys.argv[1] if len(sys.argv) > 1 else "src/llama.h"
output = sys.argv[2] if len(sys.argv) > 2 else "src/exports.def"
libname = sys.argv[3] if len(sys.argv) > 3 else "llama_cpu"

if not os.path.exists(header):
    print(f"❌ Header not found: {header}")
    sys.exit(1)

with open(header, "r", encoding="utf-8", errors="ignore") as f:
    text = f.read()

symbols = sorted(set(re.findall(r"\b(llama_[A-Za-z0-9_]+)\s*\(", text)))

with open(output, "w", encoding="ascii", errors="ignore") as f:
    f.write(f"LIBRARY {libname}\nEXPORTS\n")
    for s in symbols:
        f.write(f"    {s}\n")

print(f"✅ Generated {output} with {len(symbols)} exports.")
