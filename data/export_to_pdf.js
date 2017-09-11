// execute with phantomjs filename
var system = require('system');
var args = system.args;
var page = require('webpage').create();

if (args.length === 1) {
	console.log('Usage: ' +  args[0] + ' filename');
	phantom.exit(0);
} else {
	page.open(args[1], function() {
  		page.render(args[1]+'.pdf',{format: 'pdf'});
  		phantom.exit();
	});
}