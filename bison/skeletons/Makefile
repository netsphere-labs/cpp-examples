
OUTPUT = glr-c.tab.c glr-c.tab.h \
    glr-cc.tab.cc glr-cc.tab.hh \
    glr2-cc.tab.cc glr2-cc.tab.hh glr2-loc.hh \
    lalr1-cc.tab.cc lalr1-cc.tab.hh location.hh \
    lalr1-d.d \
    lalr1-java.java \
    yacc-c.tab.cc yacc-c.tab.hh

all: $(OUTPUT)

# YYSKELETON_NAME "glr.c"
glr-c.tab.c glr-c.tab.h: glr-c.y
	bison $<

# YYSKELETON_NAME "glr.cc"
glr-cc.tab.cc glr-cc.tab.hh: glr-cc.y
	bison $<

# 拡張子 .yy でなければならない!
# YYSKELETON_NAME "glr2.cc"
glr2-cc.tab.cc glr2-cc.tab.hh glr2-loc.hh: glr2-cc.yy
	bison $<

# `lalr1.cc` スケルトン. YYSKELETON_NAME は定義されない. なぜ?
lalr1-cc.tab.cc lalr1-cc.tab.hh location.hh: lalr1-cc.y
	bison $<

# public static immutable string yy_bison_skeleton = "lalr1.d";
lalr1-d.d: lalr1-d.y
	bison $<

# public static final String bisonSkeleton = "lalr1.java";
lalr1-java.java: lalr1-java.y
	bison $<

# YYSKELETON_NAME "yacc.c"
# 拡張子を .yy にすると出力が .cc になるが、スケルトンは yacc.c のまま。
yacc-c.tab.cc yacc-c.tab.hh: yacc-c.yy
	bison $<

clean:
	rm -f $(OUTPUT)
