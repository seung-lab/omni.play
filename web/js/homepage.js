	
	function init() {
		// animate the homepage
		startHomepage();
	}
	
	// animate the hompage logo and login
	function startHomepage() {
		// get window height
		var winHeight = $(window).height();
		// alert(winHeight);
		var headerHeight = $('#pageHeader').outerHeight(true);
		// alert(headerHeight);
		var pusherHeight = ((winHeight - headerHeight) / (2 * 1.75));
		$('#pusher').css('height', pusherHeight);
		
		// fadein logo
		$('#logo').fadeIn(1000, function() {
			$('#logo-subtitle').fadeIn(1000, function() {
				$('#pusher').animate({'height': pusherHeight - 100}, 600, null, function() {
					$('#loginContainer').fadeIn(1000);
				})
			});
		});
	}
	
	
	// toggle the login password field
	function toggleLoginPasswordField(toggle) {
		if(toggle) {
			// display password
			$id('login-password-fake').style.display = 'none';
			$id('login-password').style.display = 'inline';
			$id('login-password').focus();
		} else {
			// hide password if the value is nothing
			if($id('login-password').value == '') {
				$id('login-password-fake').style.display = 'inline';
				$id('login-password').style.display = 'none';
			}
		}
	}
	
	
	function login() {
		
	}
	
	
	function termLogin() {
		
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	