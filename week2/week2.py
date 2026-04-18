import enum

# --- 1. Token 定義 ---
class TokenType(enum.Enum):
    LET = "LET"; FN = "FN"; IF = "IF"; ELSE = "ELSE"; RETURN = "RETURN"
    IDENT = "IDENT"; NUMBER = "NUMBER"; TYPE = "TYPE"
    PLUS = "+"; MINUS = "-"; STAR = "*"; SLASH = "/"; EQ = "="
    LPAREN = "("; RPAREN = ")"; LBRACE = "{"; RBRACE = "}"; COLON = ":"
    SEMICOLON = ";"; EOF = "EOF"

class Token:
    def __init__(self, type, value):
        self.type = type
        self.value = value

# --- 2. 詞法分析器 (Lexer) ---
class Lexer:
    def __init__(self, text):
        self.text = text
        self.pos = 0

    def get_tokens(self):
        # 這裡簡略實作，實際需遍歷字串並產出 Token 列表
        # 具體包含識別關鍵字、數字與識別字
        pass

# --- 3. 虛擬機運算指令 (Bytecode/OpCodes) ---
class OpCode(enum.Enum):
    PUSH_CONST = "PUSH_CONST"  # 將常數壓入堆疊
    LOAD_VAR   = "LOAD_VAR"    # 從變數表載入
    STORE_VAR  = "STORE_VAR"   # 存入變數表
    ADD        = "ADD"         # 彈出兩個值相加後壓回
    SUB        = "SUB"
    CALL       = "CALL"
    RET        = "RET"

# --- 4. 堆疊機虛擬機 (Virtual Machine) ---
class VirtualMachine:
    def __init__(self, code):
        self.code = code  # 指令列表
        self.stack = []   # 資料堆疊
        self.env = {}     # 符號表 (變數存儲)

    def run(self):
        ip = 0 # Instruction Pointer
        while ip < len(self.code):
            op, val = self.code[ip]
            
            if op == OpCode.PUSH_CONST:
                self.stack.append(val)
            elif op == OpCode.ADD:
                b = self.stack.pop()
                a = self.stack.pop()
                self.stack.append(a + b)
            elif op == OpCode.STORE_VAR:
                self.env[val] = self.stack.pop()
            elif op == OpCode.LOAD_VAR:
                self.stack.append(self.env[val])
            
            ip += 1
        return self.stack[-1] if self.stack else None

# --- 5. 快速測試範例 ---
# 模擬編譯後的代碼: let x = 10 + 20;
example_bytecode = [
    (OpCode.PUSH_CONST, 10),
    (OpCode.PUSH_CONST, 20),
    (OpCode.ADD, None),
    (OpCode.STORE_VAR, "x"),
    (OpCode.LOAD_VAR, "x")
]

vm = VirtualMachine(example_bytecode)
result = vm.run()
print(f"VM 執行結果: {result}") # 輸出 30
