<?php
    // this small script runs some code to tell WMD that the user successfully accessed the protected resource
    // we create a randomly named temp file and return its name in a cookie. WMD will check that the cookie exists 
    // and that its contents have a matching temp file. No other information is exchanged in the file.

    if (!empty($_REQUEST['base_path']) && !empty($_REQUEST['cookie_domain'])) {
        $tmp_dir = substr($_SERVER['SCRIPT_FILENAME'],0,strpos($_SERVER['SCRIPT_FILENAME'],'wa_login.php')) . 'tmp';
        $tempnam = tempnam($tmp_dir,'');
        $fp = fopen($tempnam,'w');
	fwrite($fp, $_SERVER['WEBAUTH_USER']);
        fclose($fp);

        $fname = substr($tempnam,strrpos($tempnam,'/')+1);
        setcookie('wmd4_login',$fname,0,$_REQUEST['base_path'],$_REQUEST['cookie_domain'],TRUE,TRUE);
        $subloc = '/webauth/login';
        if (isset($_REQUEST['destination'])) $subloc .= '?destination='.$_REQUEST['destination'];
    } else {
        $subloc = '/user';
    }
    header('Location: '.substr($_SERVER['REQUEST_URI'],0,strpos($_SERVER['REQUEST_URI'],'/sites')).$subloc);

?>

