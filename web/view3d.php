<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<title>Omni.Web</title>

		<link href="./css/general.css" rel="stylesheet" type="text/css" />
		<link href="./css/homepage.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_header.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_contents.css" rel="stylesheet" type="text/css" />
		<link href="./css/page_footer.css" rel="stylesheet" type="text/css" />

		<link href="./js/jquery_themes/css/smoothness/jquery-ui-1.8.10.custom.css" rel="stylesheet" type="text/css"/>

		<link href="./css/view.css" rel="stylesheet" type="text/css" />

		<script src="http://code.jquery.com/jquery-1.4.4.min.js" type="text/javascript"></script>
		<script src="js/lib/util.js" type="text/javascript"></script>
		<script src="js/lib/debug.js" type="text/javascript"></script>
		<script src="js/lib/jquery-ui-1.8.9.custom.min.js" type="text/javascript"></script>

		<script src="./js/lib/jquery.mousewheel.min.js" type="text/javascript"></script>

<!-- from https://developer.mozilla.org/en/WebGL/Adding_2D_content_to_a_WebGL_context -->
<script src="./js/webGL/sylvester.js" type="text/javascript"></script>
<script src="./js/webGL/glUtils.js" type="text/javascript"></script>
<script src="./js/webGL/view3d.js" type="text/javascript"></script>
<script id="shader-fs" type="x-shader/x-fragment">
  void main(void) {
    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
  }
</script>
<script id="shader-vs" type="x-shader/x-vertex">
  attribute vec3 aVertexPosition;

  uniform mat4 uMVMatrix;
  uniform mat4 uPMatrix;

  void main(void) {
    gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition, 1.0);
  }
</script>


	</head>

	<body onload="init()">
		<div class="wrapper-internal">
			<div id="pageHeader" class="page_header">

				<div id="logoContainer">
					<div id="logo" class="logo" style="float: left;" onclick="window.location = './index.php';">
					</div>
					<div style="float: right;">
						<input type="button" value="Logout" style="font-size: 17px;" onclick="window.location = './index.php';">
					</div>
					<div style="clear: both;"></div>
				</div>

			</div>

			<div class="page_contents_container">
				<div class="page_contents">

					<div class="view">
						<table>
							<tbody>
								<tr>
									<td class="toolbar_column" style="">
										<div class="left_panel panel" style="">
											<div class="section">
     3D
											</div>
										</div>
									</td>
									<td class="view_canvas_column">
										<div id="viewCanvasContainer" class="view_canvas_container view_canvas_container-load">
<!-- from https://developer.mozilla.org/en/WebGL/Adding_2D_content_to_a_WebGL_context -->
     <canvas id="glcanvas" width="640" height="480">
     Your browser doesn't appear to support the HTML5 <code>&lt;canvas&gt;</code> element.
  </canvas>
										</div>
									</td>
									<td style="">
										<div class="right_panel panel" style="">
											<div class="section">
												<div class="header">
													Threshold level:
												</div>
												<div id="thresholdSlider" class="threshold_slider"></div>
											</div>
											<div class="section">
												<div class="header">
													<input type="button" value="Join" class="input-join" />
												</div>
											</div>
											<div class="section">
												<div class="header">
													<input type="button" value="Split" class="input-split" />
												</div>
											</div>
										</div>
									</td>
								</tr>
							</tbody>
						</table>

					</div>

				</div>
			</div>

			<div class="page_footer">
				<div class="bottom_nav">
					<ul class="left">
						<li class="item">
							<div><a>Home</a></div>
						</li>
						<li class="item">
							<div><a>Research</a></div>
						</li>
						<li class="item">
							<div><a>About</a></div>
						</li>
					</ul>
					<ul class="right">
						<li class="item">
							<div>&#169; Omni.Web 2011</div>
						</li>
					</ul>
				</div>
			</div>
		</div>
	</body>
</html>