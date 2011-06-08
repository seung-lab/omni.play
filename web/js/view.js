
var omni = {};
omni.sliceNum = 75;
omni.totNumChannels = 150;

omni.viewHeight = null;
omni.viewWidth = null;
omni.scaleRatio = 1;

// the view canvas and window
omni.canvas = {
    windowWidth: 600,
    windowHeight: 600,
    canvasWidth: 1500,
    canvasHeight: 1500
}

// current dimensions and information for tiles
omni.tile = {
    height: null,
    width: null,
    numRows: null,
    numCols: null,
    mip: null
};

// fake values
omni.tile = {
    height: 128,
    width: 128,
    numRows: 10,
    numCols: 10,
    mip: null
};

// omni project
omni.project = {}

var altScroll = false;
var shiftKey = false;

function init() {
    // getProjectData(loadProject);
    loadProject();
    // add event handlers
    addViewEventHandlers();
}

function loadProject() {
    // create the tiles for the current mip level
    buildTilesTable();
    showCurrentSlice();
}

function getProjectData(callback) {
    $.post('./metadata/project.php', function(data){
	omni.project = $.parseJSON(data);
	if(callback) {
	    callback.call();
	}
    }
	  );
}

function addViewEventHandlers() {
    $('#viewCanvas').draggable({
	drag: updateTiles
    });

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
	    $('#joinButton').click(function(event) {
		displayNextSlice();
	    });
	    $('#splitButton').click(function(event) {
		displayPrevSlice();
	    });

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

    // segmentation slider
    $('#segmentationSlider').slider({
	'value': 50,
	'slide': handleSegmentationAlphaSlide,
	'stop': handleSegmentationAlphaSlide
    });

    $('#thresholdSlider').slider({
	'value': 50,
	'stop': handleThresholdSlide
    });

    $('#segmentationCanvas').css({
	'opacity': 0.50,
	'filter': 'alpha(opacity=' + 50 + ');'
    });

    // select segmentation
    $('#segmentationCanvas').click(function(event) {
	if(shiftKey) {
	    handleSelectSegment(event);
	}
    });
	
	// fade in and out for segmentation alpha slider
	$('#viewCanvasContainer').bind('mouseenter', function(event) {
		$('#toolboxContainer').fadeIn('fast');
	})
	$('#viewCanvasContainer').bind('mouseleave', function(event) {
		$('#toolboxContainer').fadeOut('slow');
	})
	
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
    if(omni.sliceNum >= (omni.totNumChannels - 1)) {
	return;
    } else {
	omni.sliceNum += 1;
    }
    showCurrentSlice();
}

// prev slice
function displayPrevSlice() {
    // down
    if(omni.sliceNum == 0) {
	return;
    } else {
	omni.sliceNum -= 1;
    }
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

// display the current channel
function showCurrentSlice() {
	return;
    var channelImgUrl = './engine/channel.php?slice_num=' + omni.sliceNum;
    var segmentationImgUrl = './engine/segmentation.php?slice_num=' + omni.sliceNum;

    // set dimensions
    var img = new Image();
    img.onload = function() {
		$('#viewCanvas').css('width', this.width * omni.scaleRatio);
		$('#viewCanvas').css('height', this.height * omni.scaleRatio);

		$('#channelCanvas').css('width', this.width * omni.scaleRatio);
		$('#channelCanvas').css('height', this.height * omni.scaleRatio);

		$('#segmentationCanvas').css('width', this.width * omni.scaleRatio);
		$('#segmentationCanvas').css('height', this.height * omni.scaleRatio);

		$('#tile').css('width', this.width * omni.scaleRatio);
		$('#tile').css('height', this.height * omni.scaleRatio);

		omni.viewHeight = this.height;
		omni.viewWidth = this.width;

		$('#segmentationCanvas').css('background-image', 'url(' + segmentationImgUrl + ')');
    }

    img.src = segmentationImgUrl;

    var channelImg = new Image();
    channelImg.onload = function() {
		$('#channelCanvas').css('background-image', 'url(' + channelImgUrl + ')');
    }
    channelImg.src = channelImgUrl;
}


// build a tiles table
function buildTilesTable() {
    if(!isSet(omni.tile.height) || !isSet(omni.tile.width) || !isSet(omni.tile.numRows) || !isSet(omni.tile.numCols)) {
		return false;
    }
    var channelHtml = '<div class="tiles"><table cellpadding="0" cellspacing="0"><tbody>';
    var segmentationHtml = '<div class="tiles"><table cellpadding="0" cellspacing="0"><tbody>';
    for(var i = 1; i <= omni.tile.numRows; i++) {
		channelHtml += '<tr>';
		segmentationHtml += '<tr>';
		for(var j = 1; j <= omni.tile.numCols; j++) {
			channelHtml += '<td><div id="channel-tile-' + i + '-' + j + '" class="tile"></div></td>';
			segmentationHtml += '<td><div id="segmentation-tile-' + i + '-' + j + '" class="tile"></div></td>';
		}
		channelHtml += '</tr>';
		segmentationHtml + '</tr>';
    }
    channelHtml += '</tbody></table></div>';
    segmentationHtml += '</tbody></table></div>';
    // put it in channelCanvas and segmentationCanvas
    $('#channelCanvas').html(channelHtml);
    $('#segmentationCanvas').html(segmentationHtml);

    $('.tile').css('width', omni.tile.width + 'px');
    $('.tile').css('height', omni.tile.height + 'px');
}


// updates the tiles as the user moves the draggable canvas
function updateTiles(event, ui) {
    var ids = getVisibleTileIds(event, ui);
	// AlertPropertyNames(ids);
    $('#__visibles').val(ids);
    var channelIds = '';
    var segmentationIds = '';
	var x, y;    
	for(var key in ids) {
//	channelIds += '#channel-tile-' + ids[key] + ',';
//	segmentationIds += '#segmentation-tile-' + ids[key] + ',';
        channelId = '#channel-tile-' + ids[key].top + '-' + ids[key].left;
		// PrintText(channelId);
        segmentationId = '#segmentation-tile-' + ids[key].top + '-' + ids[key].left;
		x = ids[key].left - 1;
		y = ids[key].top - 1;
        $(channelId).css('background-image', 'url(engine/channel_tile.php?slice_num=75&tileX=' + x + '&tileY=' + y + ')');
        $(segmentationId).css('background-image', 'url(engine/segmentation_tile.php?slice_num=75&tileX=' + x + '&tileY=' + y + ')');
    }
//    $(channelIds).addClass('tile_bg');
//    $(segmentationIds).addClass('tile_bg');
}



// get a list of tile ids that are visible based on the current position of the canvas
function getVisibleTileIds() {
    var canvasPos = {
	top: parseInt($('#viewCanvas')[0].style.top),
	left: parseInt($('#viewCanvas')[0].style.left)
    };
    var top = Math.ceil(-canvasPos.top / omni.tile.height);
    // $('#__top').val(canvasPos.top + ' : ' + top);
    // if(top < 0) {
    // 	top = 0;
    // }
    var left = Math.ceil(-canvasPos.left / omni.tile.width);
    // $('#__left').val(canvasPos.left + ' : ' + left);
    // if(left < 0) {
    // 	left = 0;
    // }
    var bottom = Math.floor((omni.canvas.canvasHeight - omni.canvas.windowHeight - (-canvasPos.top)) / omni.tile.height)
    // $('#__top').val(bottom);
	if(bottom < 0) {
		bottom = omni.tile.numRows;
	}
    var right = Math.floor((omni.canvas.canvasWidth - omni.canvas.windowWidth - (-canvasPos.left)) / omni.tile.width)
	if(right < 0) {
		right = omni.tile.numCols;
	}
    // $('#__left').val(right);

    $('#__top').val(top + ' : ' + left);
    $('#__left').val(bottom + ' : ' + right);

    var ids= [];

    for(var i = 0; i <= (bottom - top); i++) {
		for(var j = 0; j <= (right - left); j++) {
			ids.push({
				top: top + i,
				left: left + j
			})
		}
    }

    return ids;
}


// converts tile coordinates to tile id
function tileCoordToId(top, left) {
    var id = (top - 1) * omni.tile.numCols + left;
    return id;
}



// handle selecting segment
function handleSelectSegment(event) {
    // AlertPropertyNames(event);
    // AlertPropertyNames(event.originalEvent);
    $.get('./actions/select_segment.php',
	  { x: event.offsetX,//event.layerX,
	    y: event.offsetY,//event.layerY,
	    slice_num: omni.sliceNum,
	    h: $('#segmentationCanvas').height(),
	    w: $('#segmentationCanvas').width()
	  },
	  function(uuid){
	      var segmentationImgUrl = './engine/segmentation.php?get_uuid=' + uuid;
	      $('<img />')
		  .attr('src', segmentationImgUrl )
		  .load(function(){
		      $('#segmentationCanvas').css('background-image', 'url(' + segmentationImgUrl + ')');
		  });
	  }
	 );
}

// segmentation slider
function handleSegmentationAlphaSlide(event, ui) {
    // AlertPropertyNames(event);
    // AlertPropertyNames(ui);
    // PrintText($(this).slider('option', 'value'));
    var opacity = $(this).slider('option', 'value') / 100.0;
    $('#segmentationCanvas').css({
	'opacity': opacity,
	'filter': 'alpha(opacity=' + opacity * 100 + ');'
    });
}

// threshold slider
function handleThresholdSlide(event, ui) {
    var thresholdVal = $(this).slider('option', 'value');
    $.get('./actions/change_threshold.php',
	  { segmentationID: 1,
	    threshold: thresholdVal
	  },
	  function(data){
	      alert(data);
	  }
	 );
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
