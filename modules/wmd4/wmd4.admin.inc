<?php
/**
 * wmd4.admin.inc
 */

function wmd4_admin_settings() {

	$form['wmd4_local_label'] = array(
		'#type' => 'textfield',
		'#title' => t('Local accounts label'),
		'#default_value' => variable_get('wmd4_local_label','Local User Login'),
		'#size' => 80,
		'#maxlength' => 255,
		'#description' => t('Label for local login field group (username and password) on Drupal user page'),
	);
	$form['wmd4_link_text'] = array(
		'#type' => 'textfield',
		'#title' => t('WebLogin Link Text'),
		'#size' => 80, '#maxlength' =>255,
		'#default_value' => variable_get('wmd4_link_text','Login with WebAuth'),
		'#description' => t('The text which is displayed as the link to the WebLogin page for SUNetID users.')
	);
	$form['wmd4_destination'] = array(
		'#type' => 'textfield',
		'#title' => t('Post-Login Destination'),
		'#size' => 80, '#maxlength' => 255,
		'#default_value' => variable_get('wmd4_destination', ''),
		'#description' => t('Drupal page to which the browser redirects after successful weblogin. (Ex: \'node/add/page\' will automatically redirect user to a new content page.) If unsure, just leave blank to keep Drupal\'s default behavior.')
	);
        $form['wmd4_post_logout'] = array(
            '#type' => 'textfield',
            '#title' => t('Post-Logout Destination'),
            '#size' => 80,
            '#maxlength' => 255,
            '#default_value' => variable_get('wmd4_post_logout',''),
            '#description' => t('Page to which the browser redirects after Drupal user_logout() is called. Value can blank for default Drupal redirect to front page, "user" to return to main login screen, "http://weblogin.stanford.edu/logout" to force deletion of single-sign-on credential, or any other page in your system.'),
        );
        $form['wmd4_close_browser_warning'] = array(
            '#type' => 'checkbox',
            '#title' => t('Display "close browser" warning on logout'),
            '#description' => t('Does a drupal_set_message warning the user to close their browser to logout completely, and also presents a link to "http://weblogin.stanford.edu/logout".'),
            '#default_value' => variable_get('wmd4_close_browser_warning', 1),
        );
        $form['wmd4_failure_msg'] = array(
                '#type' => 'textfield',
                '#title' => t('WebAuth Failure Message'),
                '#size' => 80, '#maxlength' => 255,
                '#default_value' => variable_get('wmd4_failure_msg','WebAuth authentication failed.'),
                '#description' => t('The text which is displayed as a Drupal error message if WebAuth authentication for the user fails.')
        );
	$form['wmd4_check_interval'] = array(
	    '#type' => 'textfield',
	    '#title' => t('Access and Workgroup Check Interval'),
	    '#size' => 10, '#maxlength' => 10,
	    '#default_value' => variable_get('wmd4_check_interval',10),
	    '#description' => t("The interval, in minutes, after which the system should check the user's workgroup memberships and account access. Setting to zero (not recommended) will perform this check on every page access."),
	);

	return system_settings_form($form);
}

function wmd4_admin_groups($form,$form_state) {
  $form = array();
  $groups = array();
  $table = array();
  $submitted = !empty($form_state['post']);

  $result = db_query('SELECT rid, name FROM {role} WHERE rid > 2');
  while ($role = $result->fetchObject()) {
    $roles[$role->rid] = $role->name;
  }

  $form['new_rid'] = array(
    '#name' => 'new_rid',
    '#type' => 'select',
    '#options' => $roles,
  );

  $form['new_group'] = array(
    '#name' => 'new_group',
    '#type' => 'textfield',
    '#default_value' => '',
  );

  $form['new_submit'] = array(
    '#name' => 'new_submit',
    '#type' => 'submit',
    '#value' => t('Add Mapping'),
  );

  if (!$submitted) {
    $table[] = array(drupal_render($form['new_rid']), drupal_render($form['new_group']), drupal_render($form['new_submit']));
  }
  else {
    // Keep the UI consistent.
    $form['new_rid']['#prefix'] = '<table><tr class="odd"><td>';
    $form['new_rid']['#suffix'] = '</td><td>';
    $form['new_group']['#suffix'] = '</td><td>';
    $form['new_submit']['#suffix'] = '</td><td>';
    $form['cancel'] = array(
      '#type' => 'markup',
      '#value' => l(t('Cancel'), 'admin/config/webauth/mappings'),
      '#suffix' => '</td></tr></table>',
    );
  }
  $result = db_query("SELECT wr.warid, r.name, wr.wa_group as `group` FROM {wmd4_roles} wr INNER JOIN {role} r ON wr.rid = r.rid WHERE r.rid > 2");
  while ($group = $result->fetchObject()) {
    $button_id = 'remove_warid_' . $group->warid;
    $form[$button_id] = array(
      '#name' => $button_id,
      '#type' => 'submit',
      '#value' => t('Remove Mapping'),
    );
    $row = array($group->name, $group->group, drupal_render($form[$button_id]));
    $table[] = $row;
  }
  $header = array(
    t('Drupal Role'),
    t('SUNet Group'),
    t('Action')
  );
  $form['output'] = array(
    '#markup' => theme('table', array(
      'header' => $header,
      'rows' => $table,
    )),
  );

  return $form;

}

function wmd4_admin_groups_validate($form, &$form_state) {
  if (module_exists('devel_themer')) {
    form_set_error('new_group',t('The Devel Themer module is enabled and will interfere with creation of .htaccess. Please disable it.'));
    return;
  }
  if ($form_state['clicked_button']['#value'] == t('Add Mapping')) {
    if (preg_match("/^[-_\w\d\~]+:[\w\d-_]+$/", $form_state['values']['new_group']) == 0) {
      form_set_error('new_group', t('There was an error in your group name.  Please make sure you entered it correctly.'));
    }
    $query = db_query("SELECT * FROM {wmd4_roles} WHERE rid = :rid AND wa_group = :wa_group",
      array(':rid' => $form_state['values']['new_rid'], ':wa_group' => $form_state['values']['new_group']));
    if ($query->fetchField()) {
      form_set_error('new_group', t('This webauth group is already mapped to this Drupal role!'));
    }
  }
}

function wmd4_admin_groups_submit($form, &$form_state) {

if ($form_state['clicked_button']['#value'] == t('Add Mapping')) {
    // Add mapping
    $id = db_insert('wmd4_roles')
      ->fields(array(
        'rid' => $form_state['values']['new_rid'],
        'wa_group' => $form_state['values']['new_group'],
      ))
      ->execute();
    $role_name = db_query("SELECT name FROM {role} WHERE rid = :rid", array(':rid' => $form_state['values']['new_rid']))->fetchField();
    drupal_set_message(t('Added new WebAuth mapping: @role_name => @new_group', array('@role_name' => $role_name, '@new_group' => $form_state['values']['new_group'])));
  }
  elseif ($form_state['clicked_button']['#value'] == t('Remove Mapping')) {
    // Remove mapping
    $warid = drupal_substr($form_state['clicked_button']['#name'], 13);
    db_delete('wmd4_roles')
      ->condition('warid', $warid)
      ->execute();
    drupal_set_message(t('Removed WebAuth mapping from table. [ID: @warid]', array('@warid' => $warid)));
    // Clear out any roles that were granted as a result.
    $result = db_query("SELECT * FROM {wmd4_roles_history} WHERE warid = :warid", array(':warid' => $warid));
    while ($history = $result->fetchObject()) {
      // See if there's another mapping for this role.
      if (!db_query("SELECT * FROM {wmd4_roles_history} WHERE uid = :uid AND rid = :rid AND warid <> :warid", array(':uid' => $history->uid, ':rid' => $history->rid, ':warid' => $warid))->fetchField()) {
        // If not, nix the users_roles value so they lose privileges immediately.
        db_delete('users_roles')
          ->condition('uid', $history->uid)
          ->condition('rid', $history->rid)
          ->execute();
      }
    }
    db_delete('wmd4_roles_history')
      ->condition('warid', $warid)
      ->execute();
  }

  // Need to update the .htaccess file.
  wmd4_write_htaccess();
}

function wmd4_admin_users($form_state) {

  $form = array();

  $form['wmd4_require_valid_user'] = array(
    '#type' => 'checkbox',
    '#title' => t('Require ANY valid user'),
    '#description' => t('Require the valid-user statement in settings. This overrides whatever users are entered below. Default is on.'),
    '#default_value' => variable_get('wmd4_require_valid_user', 1),
  );

  $form['wmd4_require_users'] = array(
    '#type' => 'textarea',
    '#title' => t('SUNet users permitted access'),
    '#description' => t('Enter one user per line.'),
    '#default_value' => variable_get('wmd4_require_users', ''),
  );

  $form['wmd4_require_privgroups'] = array(
    '#type' => 'textarea',
    '#title' => t('SUNet privgroup permitted access'),
    '#description' => t('Enter one privgroup (e.g."stanford:staff") per line.'),
    '#default_value' => variable_get('wmd4_require_privgroups', ''),
  );

  $form = system_settings_form($form);

  $form['#validate'][] = 'wmd4_admin_users_validate';
  $form['#submit'][] = 'wmd4_admin_users_submit';

  return $form;
}

/**
 * Validate that we're getting valid input in our .htaccess file.
 */
function wmd4_admin_users_validate($form, &$form_state) {
  if (module_exists('devel_themer')) {
    form_set_error('new_group',t('The Devel Themer module is enabled and will interfere with creation of .htaccess. Please disable it.')
);
    return;
  }

  $userlines = explode("\n", chop($form_state['values']['wmd4_require_users']));
  $grouplines = explode("\n", chop($form_state['values']['wmd4_require_privgroups']));

  foreach($userlines as $line) {
    $line = chop($line);
    if ($line != '' && preg_match("/^[\w\d-_]+$/", $line) != 1) {
      form_set_error('wmd4_require_users', t('!line is not a valid user format. Please enter one username per line.', array('!line' => $line)));
    }
  }
  foreach($grouplines as $line) {
    $line = chop($line);
    if ($line != '' && preg_match("/^[-_\w\d\~]+:[\w\d-_]+$/", $line) != 1) {
      form_set_error('wmd4_require_privgroups', t('!line is not a valid user format. Please enter one username per line.', array('!line' => $line)));
    }
  }

  if (trim($form_state['values']['wmd4_require_users']) == '' && trim($form_state['values']['wmd4_require_privgroups']) == '' && $form_state['values']['wmd4_require_valid_user'] == FALSE) {
    form_set_error('wmd4_require_valid_user', t('You must specify some valid means for accessing the site.'));
  }
}

function wmd4_admin_users_submit($form, &$form_state) {
  // Need to update the .htaccess file.
  wmd4_write_htaccess();
}

function wmd4_admin_ldap($form_state) {

  $form = array();

  $form['wmd4_require_ldapvars'] = array(
    '#type' => 'textarea',
    '#title' => t('LDAP Variables to be returned by WebAuth'),
    '#description' => t('Enter one user per line.'),
    '#default_value' => variable_get('wmd4_require_ldapvars', 'displayName'.chr(13).chr(10).'suAffiliation'.chr(13).chr(10).'mail'.chr(13).chr(10)),
  );

  $form = system_settings_form($form);

  $form['#validate'][] = 'wmd4_admin_ldap_validate';
  $form['#submit'][] = 'wmd4_admin_ldap_submit';

  return $form;
}

/**
 * Validate that we're getting valid input in our .htaccess file.
 */
function wmd4_admin_ldap_validate($form, &$form_state) {

  if (module_exists('devel_themer')) {
    form_set_error('new_group',t('The Devel Themer module is enabled and will interfere with creation of .htaccess. Please disable it.')
);
    return;
  }
  $ldaplines = explode("\n", chop($form_state['values']['wmd4_require_ldapvars']));

  foreach($ldaplines as $line) {
    $line = chop($line);
    if ($line != '' && preg_match("/^[\w\d-_]+$/", $line) != 1) {
      form_set_error('wmd4_require_ldapvars', t('!line is not a valid user format. Please enter one username per line.', array('!line' => $line)));
    }
  }
}

function wmd4_admin_ldap_submit($form, &$form_state) {
  // Need to update the .htaccess file.
  wmd4_write_htaccess();
}

