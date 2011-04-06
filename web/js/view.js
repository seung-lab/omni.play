
	var omni = {};
	omni.sliceNum = 75;
	omni.totNumChannels = 150;

	omni.viewHeight = null;
	omni.viewWidth = null;
	omni.scaleRatio = 1;

	// currently selected segment
	omni.selectedSeg = {
		x: null,
		y: null,
		selected: false
	};

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

		// segmentation slider
		$('#segmentationSlider').slider({
			'value': 80,
			'slide': handleSegmentationAlphaSlide,
			'stop': handleSegmentationAlphaSlide
		});

		$('#thresholdSlider').slider({
			'value': 80
		});

		$('#segmentationCanvas').css({
		        'opacity': 0.80,
			'filter': 'alpha(opacity=' + 80 + ');'
		});

		// select segmentation
		$('#segmentationCanvas').click(function(event) {
			if(shiftKey) {
				handleSelectSegment(event);
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

	// handle selecting segment
	function handleSelectSegment(event) {
	        // AlertPropertyNames(event);
	        // AlertPropertyNames(event.originalEvent);
                $.get('./server/select_segment.php',
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


	// toggle loading new view
	function loadNewView(load) {
		if(load) {
			$('#viewCanvas').hide();
			$('#viewCanvasContainer')
		} else {
			$('#viewCanvas').show();
		}
	}

































