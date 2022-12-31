
# Ruby. 不味い。

def test1
  re = Regexp.new "^(a+)+$"
  p re =~ 'a' * 1000 #+ '\x00'
  p re =~ 'a' * 20 + 'x'
  # こんな簡単なのでもCPU 100% に張り付く
  p re =~ 'a' * 30 + 'x'    # user    0m22.879s
end

def test2
  re = /^(([a-zA-Z0-9])+)+$/
  p re =~ "abcdefghijklmnopqrstuvwxyzABC"
  p re =~ "abcdefghijklmnopqrstuvwxyzABC@"   # user  0m15.809s
end

test1
#test2  
