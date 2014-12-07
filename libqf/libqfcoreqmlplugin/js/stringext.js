.pragma library

function stringext_init()
{
	//console.warn("stringext_init()");
	if (typeof String.prototype.startsWith !== 'function') {
		console.info("creating String.prototype.startsWith");
		String.prototype.startsWith = function(prefix, case_insensitive) {
			if(case_insensitive)
				return this.toLowerCase().lastIndexOf(prefix.toLowerCase(), 0) === 0;
			return this.lastIndexOf(prefix, 0) === 0;
		}
	}

	if (typeof String.prototype.endsWith !== 'function') {
		console.info("creating String.prototype.endsWith");
		String.prototype.endsWith = function(suffix, case_insensitive) {
			if(case_insensitive)
				return this.indexOf(suffix, this.length - suffix.length) !== -1;
			return this.toLowerCase().indexOf(suffix.toLowerCase(), this.length - suffix.length) !== -1;
		}
	}

	if(!String.prototype.trim) {
		console.info("creating String.prototype.trim");
		String.prototype.trim = function () {
			return this.replace(/^\s+|\s+$/g,'');
		}
	}
}
console.info("importing stringext.js");
stringext_init();
