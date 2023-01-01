# 正規表現の攻撃 (ReDoS) を防ぐか


## ReDoS

正規表現の書き方が不味く、かつライブラリで回避できないと、マッチ判定に指数 (指数関数) 時間が掛かる。例えば, <code>/^(a+)+$/</code> のような簡単な正規表現であっても爆発する。
正規表現の構文エラーではないため、正規表現のコンパイル時に完全に検出することはできないし、現状では簡単なものでもマッチ実行時まで検出されない。

<a href="https://owasp.org/www-community/attacks/Regular_expression_Denial_of_Service_-_ReDoS">Regular expression Denial of Service - ReDoS | OWASP Foundation</a>

破壊的なバックトラック (catastrophic backtracking) が発生する例と正規表現の書き改め; https://www.regular-expressions.info/catastrophic.html

対策は, lint ツールなどでの検出 (C/C++ では文字列と区別つかないので難しい)、マッチ時に時間超過で例外発生、機能性と引き換えに DFA で設計されたライブラリの利用、ぐらい。



## 既存のライブラリなど

### C/C++ 
意外と苦戦する。

|ライブラリ  |Unicode property (general category &amp; scripts) |UTF-16 |結果など                            |
|------------|-------------------------|-------|------------------------------------|
|glibc       |No                       |No     |POSIX Extended Regular Expressions. 結果OK. 100,000 でも一瞬.   |
|Boost.Regex |Some. <code>[:Nd:]</code> スクリプト名? |Yes <code>UnicodeString</code>   |ICUと併用でUnicodeサポート (正規表現エンジンは Boost 側; <code>u32regex_match()</code>). 不味い正規表現はマッチ実行時に例外発生.  |
|ICU Regex   |Yes                      |Yes    |アカン. そもそも正規表現エンジンがバグってる   |
|PCRE2       |Yes. Perl synonym <code>\p{Letter}</code> is not supported |<code>PCRE2_CODE_UNIT_WIDTH</code> で指定 |<code>pcre2_match()</code> は早いタイミングで matching error を返す. DFA モードだと限界が上がるが, 10,000 で反応が帰ってこない.   |
|RE2         |Yes. <code>\p{Greek}</code> |qt5-qtwebengine が利用. 結果OK. 100,000 でも一瞬.  |

複雑なことをしなければ libc がよい。使える正規表現は <kbd>egrep</kbd>(1) と同じ。長い入力でも耐えるのは <i>RE2</i> ぐらいしかない。しかし, RE2 は look-ahead, look-behind (まとめて lookaround) をサポートしていない。耐えずにすぐさま失敗する (エラー発生) のでも差し支えない。そうすると, Boost.Regex (例外発生), PCRE2 の <code>pcre2_match()</code> でもよい。逆に <code>pcre2_dfa_match()</code> は反応が帰ってこなくてダメ。

PCRE は 2系統ある. ソースコードレベルの互換性もない。そもそもAPI名がすべて変わってしまっている.
 - pcre-devel-8.45-1.fc37.2.x86_64   -- Fedora 37 ではもはや依存なし. 削除してOK.
      ライブラリのほうは, grep-3.7, sord-0.16.10 (RDF ライブラリ) 等々が依存.
 - pcre2-devel-10.40-1.fc37.1.x86_64   -- glib2-devel が利用.



試していないが面白そう:
 - https://www.akenotsuki.com/misc/srell/  Unicode対応テンプレートライブラリ.
 - https://github.com/laurikari/tre/  POSIX-compliant で効率的な実装.
 - https://github.com/hanickadot/compile-time-regular-expressions/ コンパイル時正規表現. Unicode プロパティ対応. スゲー

コンパイル時正規表現は、昔にも Boost.Xpressive があったが、これは書き方も独自で、キワモノだった。上の ctre は C++ ユーザ定義リテラルを活用して、一見ふつうの正規表現に見える.



### そのほか

 - Perl      まぁまぁ. 10,000 だと多少時間が掛かる. 入力テキストの長さを制約してやれば十分。
 - Python    アカン. 100 でもダメ. 反応が帰ってこない.
 - Ruby      アカン. 100 でもダメ. 反応が帰ってこない.
 - Google Chrome ブラウザ. アカンっぽい? 結果だけが帰ってこない.
 - Node.js   アカン. 100 でもダメ。 反応が帰ってこない.
 
Python 3.11.1, Ruby 3.1.1p18, Node.js 18.12.1 は DoS が成立してしまう。ダメ。



## 機能性のトレードオフ

上記のとおり, <i>RE2</i> は長い入力にも耐える。バックトラックしないので、機能性のトレードオフがある。

C# 界隈では, .NET 7 で正規表現が大幅強化, <code>RegexOptions.NonBacktracking</code> も導入された。有効にすると, 次がサポートされなくなる。要は RE2 と同じになる。
 - atomic groups <code>(?&gt;</code> 
 - backreferences
 - balancing groups <code>(?&lt;<var>name</var>&gt;</code> -- もともと .NET only.
 - conditional
 - lookarounds
 - start anchors (<code>\G</code>) 

Lookaround は POSIX 拡張正規表現にはない。先読みは ECMAScript 5.1 Edition (June 2011) にはある。

|         |肯定                                             |否定                                              |実装  |
|---------|-------------------------------------------------|--------------------------------------------------|------|
|先読み   |<code>(?=<var>expr</var>)</code> positive lookahead 肯定先読み    |<code>(?!<var>expr</var>)</code> negative lookahead 否定先読み     |Ruby: Yes, ECMAScript: Yes, Oniguruma: Yes, Python: Yes |
|戻り読み |<code>(?&lt;=<var>expr</var>)</code> positive lookbehind 肯定戻り読み |<code>(?&lt;!<var>expr</var>)</code> negative lookbehind 否定戻り読み |Ruby: Yes, ECMAScript: No, Oniguruma: Yes, Python: Yes |






## チェックサイトなど

<i>recheck</i> が優れている。https://makenowjust-labs.github.io/recheck/
日本語の紹介. 理論の説明も; https://speakerdeck.com/makenowjust/state-of-the-art-of-redos-detection-241e16d2-ad8e-462c-8c0e-b6a95a735d59

<ul>
  <li><i>safe-regex</i> npm は不十分。
  <li><a href="https://github.com/davisjam/vuln-regex-detector/">davisjam/vuln-regex-detector: Detect vulnerable regexes in your project. REDOS, catastrophic backtracking.</a>
</ul>

