/*
	PREDICTIVE SEQUENCE LEARNING
	By Erik Billing, www.cognitionreversed.com.
	====================================================================

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** 
	OVERVIEW
	=====================================================================

	This file defines two classes, Hypothesis and Psl, as well as a few 
	helper functions. Hypothesis holds  an association between a sequence 
	of characters and a target char, e.g., abs -> d. This implementation 
	of PSL is designed for demo purposes and uses strings only. 

	The Psl class holds a collection (library) of hypotheses, and 
	implements training and selection algorithms. 

	USAGE (depends on jquery)
	=====================================================================

	var psl = new Psl(); // Instantiate the Psl library

	var s = 'PSL Hello World Hello World'; // Define a source string

	psl.train(s); // Run training
	
	function run() {
		$('H1').append(psl.predict($('H1').text())); // Predict the next character in the sequence
		setTimeout(run,200);
	};

	setTimeout(run,200);
	
*/

function Hypothesis(lhs,rhs,hits,misses) {
	this.id = Hypothesis.count++;
	this.lhs = lhs;
	this.rhs = rhs;
	this.hits = hits || 1;
	this.misses = misses || 0;
};

Hypothesis.count = 0;

Hypothesis.prototype = {
	
	match: function(s) {
		if (s == undefined) {
			return 0;
		} else if (this.lhs.length == 0) {
			return this.conf();
		} else if (s.length < this.lhs.length && s == this.lhs.substring(this.lhs.length-s.length)) {
			return this.conf() / this.lhs.length * s.length;
		} else if (s.substring(s.length-this.lhs.length) == this.lhs) { //s > lhs
			return this.conf();
		} else {
			return 0;
		}
	},

	conf: function() {
		return this.lhs.length * this.hits / (this.hits+this.misses);
	},

	support: function(){
		return this.hits + this.misses;
	},

	reward: function(value) {
		this.hits += value == undefined ? 1 : value;
	},

	punish: function(value) {
		this.misses += value == undefined ? 1 : value;
	},

	toString: function() {
		return this.lhs + ' -> ' + this.rhs + ' (' + this.hits + '/' + this.misses + ')';
	}
};

function Psl(attr) {
	if (attr == undefined) attr = {};
	this.hypotheses = attr.hypotheses || [];
	this.vtFactor = attr.vtFactor || 2;
};

Psl.prototype = {
	clear: function() {
		this.hypotheses = [];
	},

	/** Train PSL
	 *  Usage: psl.train(source string,callback) or psl.train(attr).
	 *  @param attr is an object specifying {s: source string, startIndex, stopIndex, callback}.
	 */ 
	train: function(attr, callback) {
		var s = (typeof attr === 'string') && attr || attr.s;
		var startIndex = attr.startIndex || 1;
		var stopIndex = attr.stopIndex || s.length
		callback = callback || attr.callback;

		if (s == undefined) {
			return;
		} else if (attr.repeat && attr.repeat > 1) {
			for (var i = 1; i<attr.repeat; i++) {
				if (attr.loop) s += attr.s;
				else this.train({
					s: s,
					startIndex: startIndex
				});
			}
		}

		for (var i=startIndex;i<stopIndex; i++) {

			var sub = s.substring(0,i);
			var t = s[i];
			var hs = this.match(sub); //gets a confidence score for every hypothesis
			var maxh = undefined;
			var maxc = -1;
			var bestCorrect = undefined;

			for (var hi in hs) {
				var conf = hs[hi];
				if (conf > 0) {
					var h = this.hypotheses[hi];

					if (maxc < conf) {
						maxc = conf;
						maxh = h;
					}

					if (h.rhs == t) {
						h.reward();
						if (bestCorrect == undefined || bestCorrect.lhs.length < h.lhs.length) bestCorrect = h;
					}
				}
			}

			var correct = maxh && maxh.rhs == t;
			if (maxh && !correct) maxh.punish();
			
			var newh = undefined;
			
			if (!correct) {
			
				newh = this.grow(sub,bestCorrect || t); //psl learns only on failure
			
			}

			if (callback) callback(i, maxh, maxh && maxh.rhs || '', correct, newh);	
		}
	},
	
	getVtFactor: function(h) {
		if (!h) return 1;
		var l = h.lhs.length;
		var f = Math.floor(l*this.vtFactor);
		return Math.max(l+1,f);
	},

	grow: function(s, parent) {
		var h;
		if (typeof parent === 'string') {
			h = new Hypothesis(s[s.length-1],parent); //lhs is assigned here
			this.hypotheses.push(h);
		} else {
			h = new Hypothesis(s.substr(s.length-this.getVtFactor(parent),s.length),parent.rhs);
			this.hypotheses.push(h);
		}
		return h;
	},

	match: function(s) {
		return this.hypotheses.map(function(h) {
			return h.match(s);
		});
	},

	select: function(s) {
		var m = this.match(s);
		var i = indexOfMax(m);
		return m[i] > 0 ? this.hypotheses[i] : undefined;
	},

	predict: function(s) {
		var h = this.select(s); 
		return h ? h.rhs : '';
	},

	run: function(s,callback) {
		for (var i=1;i<=s.length; i++) {
			var h = this.select(s.substring(0,i));
			if (callback) callback(i,h,h == undefined ? '' : h.rhs, h && h.rhs == s[i]);
		}
	},

	dream: function(s,callback) {
		var psl = this;
		var next = function() {
			var h = psl.select(s);
			if (h != undefined) {
				s += h.rhs;
			}
			if (callback) callback(s.length,h, h == undefined ? '' : h.rhs, next);
		}
		next();
	},

	spectrogram: function(callback) {
		var spec = {};
		var maxSpred = 0;
		for (var i = 0; i<this.hypotheses.length; i++) {
			var h = this.hypotheses[i];
			if (h.lhs == '') continue;
			var c = h.lhs[h.lhs.length-1];
			if (spec[c] == undefined) spec[c] = {};
			var s = spec[c];
			if (s[h.rhs]) {
				s[h.rhs] ++;
			} else {
				s[h.rhs] = 1;
			}
			maxSpred = Math.max(maxSpred,Object.keys(s).length);
		}
		console.log(spec);
		if (callback) callback(spec,maxSpred);
	}
};

/* helpers */

function indexOfMax(arr) {
    if (arr.length === 0) {
        return -1;
    }

    var max = arr[0];
    var maxIndex = 0;

    for (var i = 1; i < arr.length; i++) {
        if (arr[i] > max) {
            maxIndex = i;
            max = arr[i];
        }
    }

    return maxIndex;
}

String.prototype.substr = function(from, to, pretext) {
	if (from >= 0) {
		return this.substring(from,to);
	} else {
		return Array(1-from).join(pretext || '#') + this.substring(0, to);
	}
}

/* Node.js interface */
if (typeof exports != 'undefined') {
	exports.Psl = Psl;
}
