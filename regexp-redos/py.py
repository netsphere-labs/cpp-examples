
# Python
#   -> 不味い

import re

def run_regex(re_pat, str):
    if re_pat.match(str):
        print("matched: ", str)
    else:
        print("not matched.")

re_pat = re.compile('^(a+)+$')
run_regex(re_pat, "a" * 1000)
run_regex(re_pat, "a" * 20 + "x")
run_regex(re_pat, "a" * 100 + "x") # 100 でも帰ってこない
    
