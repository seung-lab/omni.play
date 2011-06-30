//uility library for javascript, helper functions

// JavaScript Document

//isSet function returns true for not null AND not undefined
function isSet(obj) {
	return (obj != null && obj != undefined);
}

//isUnset function returns true for null OR not defined
function isUnset(obj) {
	return (obj == null || obj == undefined);
}

function $id(elementId, obj){
	if(obj == undefined || obj == null) {
			obj = document;
	}
	return document.getElementById(elementId);	
}


function $text(text, obj){
	if(obj == undefined || obj == null) {
			obj = document;
	}
	return document.createTextNode(text);	
}


function $tag(elementTag, obj){
	if(obj == undefined || obj == null) {
			obj = document;
	}
	var tagElement = document.getElementsByTagName(elementTag);	
	return tagElement;
}


function $element(elementTag, elementId){
	var newElement = document.createElement(elementTag);	
	var elementId = elementId || '';//if defined use it, if not put blank
	newElement.setAttribute('id', elementId);
	return newElement;
}


function $setAttrVal(obj, attrName, attrVal) {
	if($attr(obj, attrName)) {
		eval('obj.attributes.' + attrName + '.value=\'' + attrVal + '\'');
	} else {
		//create new attribute to add in
		var newAttr = document.createAttribute(attrName);
		newAttr.value = attrVal;
		obj.setAttributeNode(newAttr);
	}
}

function $urlVars() {
	var vars = [], hash;
	var hashes = window.location.href.slice(window.location.href.indexOf('?') + 1).split('&');
	for(var i = 0; i < hashes.length; i++)
	{
	    hash = hashes[i].split('=');
	    vars.push(hash[0]);
	    vars[hash[0]] = hash[1];
	}
	return vars;
}



function parseXML(text) {
	var xmlDoc;
	//Internet Explorer
	try {
		xmlDoc = new ActiveXObject('Microsoft.XMLDOM');
		xmlDoc.async = 'false';
		xmlDoc.loadXML(text);
	}
	catch(e) {
		// Firefox, Mozilla, Opera, etc.
		try {
			parser = new DOMParser();
			xmlDoc = parser.parseFromString(text, 'text/xml');
		}
		catch(e) {
			//alert(e.message);
			return;
		}
	}
	return xmlDoc;
}

// toggle default text
function toggleDefaultText(obj, text) {
	if(obj.value == text) {
		obj.value = '';
	} else {
		if(obj.value == '') {
			obj.value = text;
		}
	}
}

function checkEmailAddress(email) { 
	var re = /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
	return email.match(re) 
}








//make a post request and redirect
//
// url is the url to post to
// params is an object or array that stores the post parameters
function postRedirect(url, params, method) {
	method = method || "post"; // Set method to post by default, if not specified.
	
    // The rest of this code assumes you are not using a library.
    // It can be made less wordy if you use one.
    var form = document.createElement("form");
    form.setAttribute("method", method);
    form.setAttribute("action", url);

    for(var key in params) {
        var hiddenField = document.createElement("input");
        hiddenField.setAttribute("type", "hidden");
        hiddenField.setAttribute("name", key);
        hiddenField.setAttribute("value", params[key]);

        form.appendChild(hiddenField);
    }

    document.body.appendChild(form);    // Not entirely sure if this is necessary
    form.submit();
	
}


jQuery.fn.center = function () {
    this.css("position","absolute");
    this.css("top", ( $(window).height() - this.height() ) / 2+$(window).scrollTop() + "px");
    this.css("left", ( $(window).width() - this.width() ) / 2+$(window).scrollLeft() + "px");
    return this;
}


//inherite function
// child is the child class
// parent is the parent class to inherite everything from
// mode defines the mode of inheritance: 'join' means if a prototype function exists, then call both the existing and inherited: if in join mode, can also input a Func object that takes the two functions (existing, and newly inherited) and combines, or does something to it); 'overload' means replace the original for repeats; 'keep' means keep the original
// DO NOT USE mode = 'join'
function inherite(child, parent) {
	var val;
	var key;
	if(count(parent.prototype) > 0) {
		for(key in parent.prototype) {
			val = parent.prototype[key];
			if(!isSet(child.prototype[key])) {
				child.prototype[key] = val;
			}
		}
	}
}





/*
 * Date Format 1.2.3
 * (c) 2007-2009 Steven Levithan <stevenlevithan.com>
 * MIT license
 *
 * Includes enhancements by Scott Trenda <scott.trenda.net>
 * and Kris Kowal <cixar.com/~kris.kowal/>
 *
 * Accepts a date, a mask, or a date and a mask.
 * Returns a formatted version of the given date.
 * The date defaults to the current date/time.
 * The mask defaults to dateFormat.masks.default.
 */

var dateFormat = function () {
	var	token = /d{1,4}|m{1,4}|yy(?:yy)?|([HhMsTt])\1?|[LloSZ]|"[^"]*"|'[^']*'/g,
		timezone = /\b(?:[PMCEA][SDP]T|(?:Pacific|Mountain|Central|Eastern|Atlantic) (?:Standard|Daylight|Prevailing) Time|(?:GMT|UTC)(?:[-+]\d{4})?)\b/g,
		timezoneClip = /[^-+\dA-Z]/g,
		pad = function (val, len) {
			val = String(val);
			len = len || 2;
			while (val.length < len) val = "0" + val;
			return val;
		};

	// Regexes and supporting functions are cached through closure
	return function (date, mask, utc) {
		var dF = dateFormat;

		// You can't provide utc if you skip other args (use the "UTC:" mask prefix)
		if (arguments.length == 1 && Object.prototype.toString.call(date) == "[object String]" && !/\d/.test(date)) {
			mask = date;
			date = undefined;
		}

		// Passing date through Date applies Date.parse, if necessary
		date = date ? new Date(date) : new Date;
		if (isNaN(date)) throw SyntaxError("invalid date");

		mask = String(dF.masks[mask] || mask || dF.masks["default"]);

		// Allow setting the utc argument via the mask
		if (mask.slice(0, 4) == "UTC:") {
			mask = mask.slice(4);
			utc = true;
		}

		var	_ = utc ? "getUTC" : "get",
			d = date[_ + "Date"](),
			D = date[_ + "Day"](),
			m = date[_ + "Month"](),
			y = date[_ + "FullYear"](),
			H = date[_ + "Hours"](),
			M = date[_ + "Minutes"](),
			s = date[_ + "Seconds"](),
			L = date[_ + "Milliseconds"](),
			o = utc ? 0 : date.getTimezoneOffset(),
			flags = {
				d:    d,
				dd:   pad(d),
				ddd:  dF.i18n.dayNames[D],
				dddd: dF.i18n.dayNames[D + 7],
				m:    m + 1,
				mm:   pad(m + 1),
				mmm:  dF.i18n.monthNames[m],
				mmmm: dF.i18n.monthNames[m + 12],
				yy:   String(y).slice(2),
				yyyy: y,
				h:    H % 12 || 12,
				hh:   pad(H % 12 || 12),
				H:    H,
				HH:   pad(H),
				M:    M,
				MM:   pad(M),
				s:    s,
				ss:   pad(s),
				l:    pad(L, 3),
				L:    pad(L > 99 ? Math.round(L / 10) : L),
				t:    H < 12 ? "a"  : "p",
				tt:   H < 12 ? "am" : "pm",
				T:    H < 12 ? "A"  : "P",
				TT:   H < 12 ? "AM" : "PM",
				Z:    utc ? "UTC" : (String(date).match(timezone) || [""]).pop().replace(timezoneClip, ""),
				o:    (o > 0 ? "-" : "+") + pad(Math.floor(Math.abs(o) / 60) * 100 + Math.abs(o) % 60, 4),
				S:    ["th", "st", "nd", "rd"][d % 10 > 3 ? 0 : (d % 100 - d % 10 != 10) * d % 10]
			};

		return mask.replace(token, function ($0) {
			return $0 in flags ? flags[$0] : $0.slice(1, $0.length - 1);
		});
	};
}();

// Some common format strings
dateFormat.masks = {
	"default":      "ddd mmm dd yyyy HH:MM:ss",
	shortDate:      "m/d/yy",
	mediumDate:     "mmm d, yyyy",
	longDate:       "mmmm d, yyyy",
	fullDate:       "dddd, mmmm d, yyyy",
	shortTime:      "h:MM TT",
	mediumTime:     "h:MM:ss TT",
	longTime:       "h:MM:ss TT Z",
	isoDate:        "yyyy-mm-dd",
	isoTime:        "HH:MM:ss",
	isoDateTime:    "yyyy-mm-dd'T'HH:MM:ss",
	isoUtcDateTime: "UTC:yyyy-mm-dd'T'HH:MM:ss'Z'"
};

// Internationalization strings
dateFormat.i18n = {
	dayNames: [
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
		"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
	],
	monthNames: [
		"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
		"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
	]
};

// For convenience...
Date.prototype.format = function (mask, utc) {
	return dateFormat(this, mask, utc);
};







































































