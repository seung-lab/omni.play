//JS library used for debugging purposes



//can receive any number of html to print out on top of the webpage
//all debugging function name start with capitalized letter
function PrintHtml(){
	//creating the error box and inserting it at beginning of body
	var errorBox = $element('div', 'errorBox');
	$tag('body')[0].insertBefore(errorBox, $tag('body')[0].firstChild);	
	
	var htmlToBeInserted = '';
	for(var i=0; i<arguments.length; i++){
		htmlToBeInserted += (arguments[i] + '<br/>');
	}
	errorBox.innerHTML = htmlToBeInserted;
	
}




















/* for debugging purposes-------------------------------------------------------------------------------------------------------------------------*/

//can receive any number of html to print out on top of the webpage
//all debugging function name start with capitalized letter
function PrintHtml(){
	//creating the error box and inserting it at beginning of body
	var errorBox = $element('div', 'errorBox');
	$tag('body')[0].insertBefore(errorBox, $tag('body')[0].firstChild);	
	
	var htmlToBeInserted = '';
	for(var i=0; i<arguments.length; i++){
		htmlToBeInserted += (arguments[i] + '<br/>');
	}
	errorBox.innerHTML = htmlToBeInserted;
	
}
/*accepts any number of text and prints them out*/
function PrintText(){
	//creating the error box and inserting it at beginning of body
	var errorBox = $element('div', 'errorBox');
	$tag('body')[0].insertBefore(errorBox, $tag('body')[0].firstChild);	

	for(var i=0; i<arguments.length; i++){
		errorBox.appendChild($text(arguments[i] + '\n'));
	}
}


/*accepts any number of arrays and prints out all its values*/
function PrintArray(){

	//creating the error box and inserting it at beginning of body
	var errorBox = $element('div', 'errorBox');
	$tag('body')[0].insertBefore(errorBox, $tag('body')[0].firstChild);	

	for(var i=0; i<arguments.length; i++){
		var msg = '';
		//cycles through the array passed as each argument and print out each
		for(var j=0; j<arguments[i].length; j++)
			msg += arguments[i][j] + '\n';
		errorBox.appendChild($text(msg + '\n'));
	}
}
/*accepts any number of objects*/
function PrintObject(){
	//creating the error box and inserting it at beginning of body
	if (arguments.length != 3) {
        throw new Error("function f called with " + arguments.length +
                        "arguments, but it expects 3 arguments.");
    }
	var errorBox = $element('div', 'errorBox');
	$tag('body')[0].insertBefore(errorBox, $tag('body')[0].firstChild);	

	for(var i=0; i<arguments.length; i++){
		errorBox.appendChild($text(PropertyNames(arguments[i]) + '\n'));
	}
}


function PropertyNames(obj){
	var names = '';
    for(var name in obj) names += name + ': ' + name.value + '\n';
	return names;
}


function PrintPropertyNames(obj) {
    var names = '<table border="1px">';
    for(var name in obj) {
		names += '<tr><td width="200px">' + name + ': ' + '</td><td>' + obj[name] + '</td></tr>';
	}
	names += '</table>';
    PrintHtml(names);
}

function AlertPropertyNames(obj) {
    var names = '';
    for(var name in obj) names += name + ': ' + obj[name] + '\n';
    alert(names);
}






















function printStackTrace() {
  var callstack = [];
  var isCallstackPopulated = false;
  try {
    i.dont.exist+=0; //doesn't exist- that's the point
  } catch(e) {
    if (e.stack) { //Firefox
      var lines = e.stack.split('\n');
      for (var i=0, len=lines.length; i<len; i++) {
        if (lines[i].match(/^\s*[A-Za-z0-9\-_\$]+\(/)) {
          callstack.push(lines[i]);
        }
      }
      //Remove call to printStackTrace()
      callstack.shift();
      isCallstackPopulated = true;
    }
    else if (window.opera && e.message) { //Opera
      var lines = e.message.split('\n');
      for (var i=0, len=lines.length; i<len; i++) {
        if (lines[i].match(/^\s*[A-Za-z0-9\-_\$]+\(/)) {
          var entry = lines[i];
          //Append next line also since it has the file info
          if (lines[i+1]) {
            entry += " at " + lines[i+1];
            i++;
          }
          callstack.push(entry);
        }
      }
      //Remove call to printStackTrace()
      callstack.shift();
      isCallstackPopulated = true;
    }
  }
  if (!isCallstackPopulated) { //IE and Safari
    var currentFunction = arguments.callee.caller;
    while (currentFunction) {
      var fn = currentFunction.toString();
      var fname = fn.substring(fn.indexOf(";function") + 8, fn.indexOf('')) || 'anonymous';
      callstack.push(fname);
      currentFunction = currentFunction.caller;
    }
  }
  output(callstack);
}

function output(arr) {
  //Optput however you want
  alert(arr.join('\n\n'));
}
































