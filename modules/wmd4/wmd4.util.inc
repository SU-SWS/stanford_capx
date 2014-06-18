<?php

// WMD4 Utility Functions
define('WEBAUTH_DOMAIN', 'stanford.edu');

/**
 * call the _wmd4_udpate_main_htaccess routine from a Drupal admin tab
 * then return to the main wmd4 tab
 */
function _wmd4_update_main_htaccess_page() {
    _wmd4_update_main_htaccess();
    drupal_goto('admin/config/people/webauth');
}

/**
 * check the main .htaccess file for WebAuth directives and add any
 * needed for WMD4
 */
function _wmd4_update_main_htaccess() {
    $todo = array();
    $file = file_get_contents('.htaccess'); // get the main directory file
    _wmd4_htaccess_preg_search('authtype ','WebAuth',$file,$todo);
    _wmd4_htaccess_preg_search('require ','valid-user',$file,$todo);
    _wmd4_htaccess_preg_search('webauthoptional ','on',$file,$todo);

    if (empty($todo)) {
        drupal_set_message('Main .htaccess file checked. No updates necessary.');
    } else {
        $d6_htaccess = substr($_SERVER['SCRIPT_FILENAME'],0,strpos($_SERVER['SCRIPT_FILENAME'],'index.php')) . '.htaccess';
        $fh = fopen($d6_htaccess,'a');
        if ($fh !== FALSE) {
            fwrite($fh,chr(10));
	    foreach ($todo as $whattodo) {
	        if ($whattodo == 'ADD_AUTHTYPE ') {
	            fwrite($fh,'AuthType WebAuth'.chr(10));
		    variable_set('wmd4_add_authtype',1);
	        } else if ($whattodo == 'ADD_REQUIRE ') {
		    fwrite($fh,'require valid-user'.chr(10));
		    variable_set('wmd4_add_validuser',1);
	        } else if ($whattodo == 'ADD_WEBAUTHOPTIONAL ') {
		    fwrite($fh,'WebAuthOptional on'.chr(10));
		    variable_set('wmd4_add_webauthoptional',1);
	        }
	    }
        }
        fclose($fh);
	drupal_set_message('Main .htaccess file checked and updated.');
    }
}

/**
 * look for general and  specific directives in the .htaccess file,
 * post warning messages as needed, and return instructions for directives
 * to be added
 */
function _wmd4_htaccess_preg_search($word1, $word2, $file, &$todo) {
    //check for word1
    $result = array(); // required dummy array
    $m1 = preg_match_all('/'.$word1.'/i',$file,$result); // see if there are any of the general directive specified by $word1
    if ($m1 === FALSE) {
        // something wrong
        drupal_set_message(check_plain('Error matching "'.$word1.'" against .htaccess file.'),'error');
    } else if ($m1 == 0) {
        // no instance of directive set, so we can add one
        $todo[] = 'ADD_' . strtoupper($word1);
    } else {
        // check specifically for "$word1 $word2" directive
        $result = array();
        $m2 = preg_match_all('/(?i)'.$word1.'(?-i)'.$word2.'/',$file,$result);
        if ($m2 === FALSE) {
            // something wrong
            drupal_set_message(check_plain('Error matching "'.$word1.' '.$word2.'" against .htaccess file.'),'error');
        } else if ($m1 > 1 || ($m1 == 1 && $m2 == 0)) {
            // there's multiple $word1 directives for something other than $word2; set a warning but allow "$word1 $word2" to be added
            drupal_set_message(check_plain('Warning - additional "'.$word1.'" directives found in .htaccess file. This may cause unpredictable results.'),'warning');
            if ($m2 == 0) $todo[] = 'ADD_' . strtoupper($word1);
        } else {
            // there's only one "$word1" directive and it is for "$word1 $word2" so do nothing
        }
    }
}

/** 
 * create a return URL and store it in the current session. this is the URL to which the browser will redirect after 
 * successful login. builds the URL from 'q' and 'destination' parameters, unless overridden with $url
 */
function _wmd4_save_return_url($url = NULL) {
    $returnDest = $url;
    if (empty($returnDest)) {
      // get the default login destination
      $wa_dest = trim(variable_get('wmd4_destination', ''));
      // if destination is set, that will be our return url
      if (isset($_GET['destination']) && strcmp($_GET['destination'], 'admin') !== TRUE) {
        $returnDest = $_GET['destination'];
      }
      // otherwise, use the default if set
      elseif (!empty($wa_dest)) {
        $returnDest = $wa_dest;
      }
      // otherwise use the current location (unless it is user/login
      elseif (isset($_GET['q']) && strcmp($_GET['q'], 'user/login') != 0) {
        $returnDest = $_GET['q'];
      }
    }

    // Save query for later use.
    $query = array();
    foreach ($_GET as $key => $value) {
      if ($key != 'q' && $key != 'destination') {
	$query[$key] = $value;
      }
    }
    $returnDest = url($returnDest,array('query' => $query, 'absolute'=>TRUE));
    $_SESSION['wa_return_url'] = $returnDest;
    return $returnDest;
}

/**
 * helper function to see if it is time to call the webauth_check
 */
function _wmd4_interval_check() {
    $interval = variable_get('wmd4_check_interval',10);
    if (!is_numeric($interval)) $interval = 10;
    $last_check = 0;
    if (isset($_SESSION['wa_last_role_check']))$last_check = $_SESSION['wa_last_role_check'];
    if (!is_numeric($last_check)) $last_check = 0;
    return (time() > ($last_check + ($interval * 60)));
}

/**
 * helper function to logout a user - since this may get called at hook_boot time
 * we need to include some core Drupal code.
 */
function _wmd4_user_logout() {
    require_once 'modules/user/user.module';
    require_once 'includes/common.inc';
    require_once 'includes/path.inc';
    global $user;
    session_destroy();
    $null = NULL;
    user_module_invoke('logout', $null, $user);
    $user = drupal_anonymous_user();
    // set a cookie to display a message after logout
    global $cookie_domain;
    $base_path = substr($_SERVER['SCRIPT_NAME'],0,strpos($_SERVER['SCRIPT_NAME'],'index.php'));
    setcookie("wmd4_auto_logout","auto",0,$base_path,$cookie_domain);
    drupal_goto(variable_get('site_403',''));
}

/**
 * helper function to create login sub-URI
 */
function _wmd4_login_q($check_maintenance = FALSE) {
    // if the site is in maintenance mode, send user directly to webauth'd script
    // otherwise, send through the webauth/login menu
    if ($check_maintenance && variable_get('maintenance_mode', 0)) {
        return conf_path().'/webauth/wa_login.php';
    } else {
        return 'webauth/login';
    }
}

/**
 * helper function to create a webauth login link
 */
function _wmd4_build_login_link() {
    global $base_url, $cookie_domain, $base_path;;
    $secure_base_url = str_replace('http:', 'https:', $base_url);
    $webauth_login = $secure_base_url . '/' . _wmd4_login_q(TRUE);

    $post_login_destination = trim(variable_get('wmd4_destination', ''));
    if (empty($post_login_destination)) {
        $destination = drupal_get_destination();
        $post_login_destination  = trim($destination['destination']);
    }
    $options = array('attributes'=>array('class'=>array('button')),'query'=>array());
    if (!empty($post_login_destination)) {
        $options['query']['destination'] = $post_login_destination;
    }
    if (variable_get('maintenance_mode',0)) {
        $options['query']['cookie_domain']=$cookie_domain;
        $options['query']['base_path']=$base_path;
    }
    $wa_url = l(variable_get('wmd4_link_text','Login with WebAuth'), $webauth_login,$options);
    return $wa_url;
}

/**
 * helper function to return login link in a Drupal form element
 */
function _wmd4_build_login_form_element() {
    $login_elem = array(
        '#prefix' => '<div id="webauth-link">',  // make it themable
        '#markup' => _wmd4_build_login_link(),
        '#suffix' => '</div>',
        '#weight' => -10
    );
    return $login_elem;
}

/**
 * helper function to check if the REQUEST_URI is for webauth login
 */
function _wmd4_check_login_uri() {
    $login_uri = base_path() . _wmd4_login_q();
    $request_uri = $_SERVER['REQUEST_URI'];
    if (strpos($request_uri,'?') !== FALSE) {
	$request_uri = substr($request_uri,0,strpos($request_uri,'?'));
    }
    return ($request_uri === $login_uri);
}

/** 
 * This is a helper function to return LDAP variables from the
 * webauth-protected check.php script. Users can call this from their
 * own modules
 */
function _wmd4_get_ldap_variables() {

    require_once 'includes/common.inc';

    $ldap_vars = FALSE;

    $headers = array('Host'=>$_SERVER['HTTP_HOST'], 'Connection'=>'close', 'Accept'=>'*/*',
        'Cookie'=>'webauth_at='.urlencode($_COOKIE['webauth_at']));
    $sock = drupal_http_request('https://'.$_SERVER['HTTP_HOST'].base_path().conf_path().'/webauth/check.php',$headers);
    if ($sock->code == 200) {
        $ldap_vars = array();
        foreach($sock->headers as $header => $value) {
            $ldap_vars[] = $header.':'.$value;
        }
    }
    return $ldap_vars;
}

/**
 * This is a helper function to grant drupal roles based on webauth data.
 *
 * This is called when new users are created via webauth, or when "local"
 * drupal users log in. It could also be invoked by other operations to
 * sync up a users roles based on their webauth settings.
 */
function _wmd4_check_access_and_groups() {

    require_once 'includes/common.inc';

    $_SESSION['wa_last_role_check'] = time();
    $groups = FALSE;

    $headers = array('Host'=>$_SERVER['HTTP_HOST'], 'Connection'=>'close', 'Accept'=>'*/*',
        'Cookie'=>'webauth_at='.urlencode($_COOKIE['webauth_at']));
    $sock = drupal_http_request('https://'.$_SERVER['HTTP_HOST'].base_path().conf_path().'/webauth/check.php',array('headers'=>$headers));
    if ($sock->code == 200) {
        $groups = array();
        foreach($sock->headers as $header => $value) {
            if (strlen($header) >= 16 && substr($header,0,16) == 'wa_ldapprivgroup') {
                if (strlen($header) == 16) {
                     $groups[] = $value;
                } else {
                     $ld_val = substr($header,16);
                     if (is_numeric($ld_val)) $groups[] = $value;
                }
            }
        }
    }
    return $groups;
}

function _wmd4_update_groups($account,$groups = array(),$user_save = TRUE) { 
    
    $new = array('roles' => $account->roles);
  
    // Add SUNet User role for all valid users.
    $default_name = db_query("SELECT name FROM {role} WHERE rid = :rid", array(':rid'=>variable_get('wmd4_default_role', 0)))->fetchField();
    $default_role = variable_get('wmd4_default_role',0);
    if ($default_role != 0 && !isset($new['roles'][$default_role])) {
      $new['roles'][$default_role] = $default_name;
    }

    $current = array();

    // Find all roles that are in the wmd4_roles table and add any ones that user is in.
    $result = db_query("SELECT r.rid, r.name, wr.wa_group AS `group`, wr.warid FROM {wmd4_roles} wr INNER JOIN {role} r ON r.rid = wr.rid");
    foreach ($result as $role) {
      if (in_array($role->group, $groups)) {
        $new['roles'][$role->rid] = $role->name;
        $current[$role->rid] = $role->rid;
	db_delete('wmd4_roles_history')->condition('warid',$role->warid)->condition('uid',$account->uid)->execute();
        db_insert('wmd4_roles_history')->fields(array('warid'=>$role->warid,'uid'=>$account->uid,'rid'=>$role->rid))->execute();
      }
    }
    // Close out any groups they shouldn't have access to.
    $result = db_query("SELECT * FROM {wmd4_roles_history} WHERE uid = :uid", array(':uid'=>$account->uid));
    foreach ($result as $role) {
      if (!isset($current[$role->rid])) {
        unset($new['roles'][$role->rid]);
	db_delete('wmd4_roles_history')->condition('uid',$account->uid)->condition('rid',$role->rid)->execute();
	db_delete('users_roles')->condition('uid',$account->uid)->condition('rid',$role->rid)->execute();
      }
    }

    require_once(drupal_get_path('module','user').'/user.module');
    if ($user_save) {
        user_save($account, $new);
    }
    $return_data = array('new'=>$new);
    return $return_data;
}

