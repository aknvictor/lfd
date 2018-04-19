// tests/part1/cart-summary-test.js
var chai = require('chai');
var expect = chai.expect; // we are using the "expect" style of Chai
var Psl = require('../psl.js').Psl;

describe('PSL null test', function() {
  it('PSL should predict "" if not trained', function() {
    var psl = new Psl();
    expect(psl.predict()).to.equal('');
  });
});

describe('PSL A test', function() {
  it('PSL should predict "B" if trained on AB', function() {
    var psl = new Psl();
    psl.train('AB');
    expect(psl.predict('A')).to.equal('B');
  });
});

describe('PSL ABCC test', function() {
  it('PSL should learn the string ABCC from repetition', function() {
    var psl = new Psl();
    var s = ['','','','c','b','c','a','c','b','c','c','a','b','c','c'];
    psl.train('abccabccabccabcc', function(i, maxh, predicted, correct, newh) {
      console.log(i + ', Selected hypothesis: ' + (maxh ? maxh.toString() : 'null') + (correct ? ' (correct) ' : ' (incorrect) ') + (newh ? ('New hypothesis: ' + newh.toString()) : ''));
    	expect(predicted).to.equal(s[i-1]);
    });
  });
});
 