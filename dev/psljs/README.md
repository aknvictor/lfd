psl.js
======

JavaScript implementation of Predictive Sequence Learning.

Examples
--------

A simple Hello world instanciation of **psl.js** is available [here](http://www.cognitionreversed.com/psltest). 

You may also check out the more elaborated example where **psl.js** is used to predict your actions in [a little maze game](http://www.cognitionreversed.com/maze).

Usage
-----

**psl.js** can be used in the browser or standalone, using [node.js](https://nodejs.org/en/). 

### Browser uage 

Download **psl.js** from [GitHub](https://github.com/billingo/psl.js) or linke directly to the CDN: 

```html
<script src="https://cdn.rawgit.com/billingo/psl.js/b7b7ab72/psl.js"></script>
```

Now, Psl can be instanciated, trained, and executed like this: 

```javascript
$(function() {
	var psl = new Psl(); // Instantiate the Psl library
	var s = 'PSL Hello World Hello World';
	psl.train({s:s});
	
	function run() {
		$('H1').append(psl.predict($('H1').text()));
		setTimeout(run,200);
	};

	setTimeout(run,200);
});
```

Please note that this example depends on [jQuery](https://jquery.com/). **psl.js** itself is however independent of external libraries. Like **psl.js**, jQuery can be linked directly from the [CDN](https://code.jquery.com/).

### Standalone usage

First, make sure you have [node.js](https://nodejs.org/en/) installed. **psl.js** is available in the [Node Package Manager (npm)](https://www.npmjs.com/). Assuming that you have npm installed, simply do: 

```
npm install psl.js
```

Alternatively, you may also download **psl.js** from [GitHub](https://github.com/billingo/psl.js) and link manually. 

Now, you make create a simple little Hello World like this:

```javascript
var Psl = require('psl.js').Psl; // Require the Psl class from psl.js
var psl = new Psl();             // Instantiate Psl

var s = 'PSL Hello World Hello World'; // Define a source string
var h = 'P';                     // Initiate the history

process.stdout.write(h);         // Write history to console

psl.train(s);                    // Run training

function run() {
	var c = psl.predict(h);  // Predict the next character in the sequence
	process.stdout.write(c); // Print predicted character to console
	h += c;                  // Append history
	setTimeout(run,200);     // Callback
};

setTimeout(run,200);         // Initiate callback
``` 

Have fun! [@billingo](https://github.com/billingo)