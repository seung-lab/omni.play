var omni = {};
omni.sliceNum = 60;
omni.totNumChannels = 120;

omni.viewHeight = null;
omni.viewWidth = null;
omni.scaleRatio = 1;

var altScroll = false;
var shiftKey = false;

function init() {
    showCurrentSlice();

    // add event handlers
    addViewEventHandlers();
}

function addViewEventHandlers() {
    $('#viewCanvas').draggable();

    $('#viewCanvasContainer').mousewheel(handleScroll);

    // altScroll
    // use command for mac (224) and alt for windows (18)
    var altKeyCode;
    if(navigator.appVersion.indexOf("Mac")!=-1) {
	altKeyCode = 224;
    } else {
	// all else
	altKeyCode = 18;
    }

    // shift key
    var shiftKeyCode = 16;

    $(document).keydown(function(event) {
	// alert(event.keyCode);
	if(event.keyCode == altKeyCode) {
	    altScroll = true;
	}
	if(event.keyCode == shiftKeyCode) {
	    shiftKey = true;
	    $('#viewCanvasContainer').css('cursor', 'default');
	}
    });
    $(document).keyup(function(event) {
	if(event.keyCode == altKeyCode) {
	    altScroll = false;
	}
	if(event.keyCode == shiftKeyCode) {
	    shiftKey = false;
	    $('#viewCanvasContainer').css('cursor', 'move');
	}
    });

    // w or s
    var wKeyCode = 119;
    var sKeyCode = 115;
    $(document).keypress(function(event) {
	// AlertPropertyNames(event);
	// AlertPropertyNames(event.originalEvent);
	// alert(event.which);
	if(event.which == wKeyCode) {
	    displayNextSlice();
	}
	if(event.which == sKeyCode) {
	    displayPrevSlice();
	}
    });
}

// handle scrolling
function handleScroll(event, delta) {
    // if command key is pressed, will scroll through channels, otherwise, zoom
    if(altScroll) {
	handleChannelScroll(event, delta);
    } else {
	handleZoomScroll(event, delta);
    }
}

// handle scrolling through channels
function handleChannelScroll(event, delta) {
    if(delta > 0) {
	// up
	displayNextSlice();
    } else if (delta < 0) {
	// down
	displayPrevSlice();
    }
    return false; // prevent default
}

// next slice
function displayNextSlice() {
    if(omni.sliceNum >= omni.totNumChannels) {
	return;
    }

    omni.sliceNum += 1;
    showCurrentSlice();
}

// prev slice
function displayPrevSlice() {
    // down
    if(omni.sliceNum == 1) {
	return;
    }

    omni.sliceNum -= 1;
    showCurrentSlice();
}

// handle zoom scroll
function handleZoomScroll(event, delta) {
    // PrintText(omni.scaleRatio);
    // change the scaling ratio and redisplay
    if(delta > 0) {
	// zoom in
	omni.scaleRatio *= 1.125;
    } else if(delta < 0) {
	// zoom out
	omni.scaleRatio /= 1.125;
    }
    showCurrentSlice();
}

// toggle loading new view
function loadNewView(load) {
    if(load) {
	$('#viewCanvas').hide();
	$('#viewCanvasContainer')
    } else {
	$('#viewCanvas').show();
    }
}

// display the current channel
function showCurrentSlice()
{
    $.get('./engine/channel.php',
          { slice_num: omni.sliceNum
          },
          function(imageUrl)
          {
              var channelImg = new Image();

              channelImg.onload = function()
              {
		  //$('#channelCanvas').css('background-image', 'url(' + imageUrl + ')');
                  $('#channelCanvasImg').attr("src",imageUrl);
                  $('#sliceNumberTitle').text("Slice Number: " + omni.sliceNum);
              }
              channelImg.src = imageUrl;
          }
         );
}
