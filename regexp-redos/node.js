
const re = /^([a-z]+)+$/

console.time('hoge');
console.log(re.test('aaaaaaaaaaaaaaaaaaaaaaaaaaaa'));
console.log(re.test('a'.repeat(10) + '!')); // 3.993ms
console.log(re.test('a'.repeat(100) + '!')); // 帰ってこない

console.timeEnd('hoge');

