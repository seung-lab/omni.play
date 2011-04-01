
	var omni = {};
	omni.sliceNum = 75;
	omni.totNumSlices = 150;

	omni.viewHeight = null;
	omni.viewWidth = null;
	omni.scaleRatio = 1;

	var altScroll = false;

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
		// var altKeyCode = 224;
                var altKeyCode = 18;
		// if windows

		$(document).keydown(function(event) {
			// alert(event.keyCode);
			if(event.keyCode == altKeyCode) {
				altScroll = true;
			}
		});
		$(document).keyup(function(event) {
			if(event.keyCode == altKeyCode) {
				altScroll = false;
			}
		});

		// segmentation slider
		$('#segmentationSlider').slider({
			'value': 75,
			'slide': handleSegmentationAlphaSlide,
			'stop': handleSegmentationAlphaSlide
		});
		$('#segmentationCanvas').css({
			'opacity': 0.75,
			'filter': 'alpha(opacity=' + 75 + ');'
		});


		$('#thresholdSlider').slider({
			'value': 50
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
			if(omni.sliceNum >= (omni.totNumSlices - 1)) {
				omni.sliceNum = omni.totNumSlices - 1;
			} else {
				omni.sliceNum += 1;
			}
		} else if (delta < 0) {
			// down
			if(omni.sliceNum == 0) {
                                omni.sliceNum = 0;
			} else {
				omni.sliceNum -= 1;
			}
		}
		showCurrentSlice();
		return false; // prevent default
	}

	// handle zoom scroll
	function handleZoomScroll(event, delta) {
		// PrintText(omni.scaleRatio);
		// change the scaling ratio and redisplay
		if(delta > 0) {
			// zoom in
			omni.scaleRatio += 0.05;
		} else if(delta < 0) {
			// zoom out
			omni.scaleRatio -= 0.05;
		}
		showCurrentSlice();
	}

	// display the current channel
	function showCurrentSlice() {
		var channelImgUrl = './engine/channel.php?slice_num=' + omni.sliceNum // + '&zoom_scale=' + omni.scaleRatio;
		$('#channelCanvas').css('background-image', 'url(' + channelImgUrl + ')');

		var segmentationImgUrl = './engine/segmentation.php?slice_num=' + omni.sliceNum // + '&zoom_scale=' + omni.scaleRatio;
		$('#segmentationCanvas').css('background-image', 'url(' + segmentationImgUrl + ')');

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
		}
		img.src = channelImgUrl;
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
