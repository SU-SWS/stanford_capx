<?php
/**
 * @file
 * Administrative pages for Stanford Earth CAP API lite module.
 */

/**
 * Form builder for settings page.
 *
 * This is based on code provided by Trellon
 */
function ses_cap_lite_settings($form, &$form_state) {

    // start with an instructive string
    $markup_str = 'Enter your credentials for the CAP API.<br /> Don\'t have credentials? File a HelpSU ticket to Administrative Applications/CAP (Stanford Profiles.';

    // get the current credentials for testing
    $clientid = variable_get('ses_cap_lite_clientid','');
    $password = variable_get('ses_cap_lite_password','');

    // check if we have encryption and if we do, decrypt the credentials, otherwise post a warning
    $encrypt = module_exists('encrypt');
    if ($encrypt) {
        $clientid = decrypt($clientid);
        $password = decrypt($password);
    } else {
        $markup_str .= '<br />We strongly recommend installing and configuring the \'encrypt\' module before proceeding, otherwise your CAP API credentials will be stored in the database as clear text.';
    }

    // see if we can authenticate the current credentials and post a message if we can
    $authd = FALSE;
    $auth_token = ses_cap_lite_auth($clientid, $password);
    if ($auth_token) {
        $markup_str .= '<br /><br />Good news! Your credentials are set and valid and your site can currently connect to the CAP API.';
    }

    // set up the form
    $form['description_wrapper'] = array(
        '#type' => 'container',
    );
    $form['description_wrapper']['description'] = array(
        '#markup' => t($markup_str),
    );
    $form['ses_cap_lite_clientid'] = array(
        '#type' => 'textfield',
        '#title' => t('Client ID:'),
        '#default_value' => $clientid,
        '#required' => TRUE,
    );

    $form['ses_cap_lite_password'] = array(
        '#type' => 'password',
        '#title' => t('Password:'),
    );
    
    $form['advanced'] = array(
        '#type' => 'fieldset',
        '#title' => t('Advanced'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#description' => t('Advanced setting for CAP API and authentication URIs'),
    );

    $form['advanced']['ses_cap_lite_base_url'] = array(
        '#type' => 'textfield',
        '#title' => t('Endpoint'),
        '#description' => t('CAP API endpoint URI, only useful when switching between development/production environment.'),
        '#default_value' => variable_get('ses_cap_lite_base_url', 'https://api.stanford.edu'),
        '#required' => TRUE,
    );

    $form['advanced']['ses_cap_lite_auth_uri'] = array(
        '#type' => 'textfield',
        '#title' => t('Authentication URI'),
        '#description' => t('CAP API authentication URI.'),
        '#default_value' => variable_get('ses_cap_lite_auth_uri', 'https://authz.stanford.edu/oauth/token'),
        '#required' => TRUE,
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save settings'),
    );

    return $form;
}

/**
 * Validation handler for settings form.
 *
 * This is based on code proved by Trellon
 */
function ses_cap_lite_settings_validate($form, &$form_state) {
  if (!empty($form_state['values']['ses_cap_lite_clientid']) && !empty($form_state['values']['ses_cap_lite_password'])) {
    $username = $form_state['values']['ses_cap_lite_clientid'];
    $password = $form_state['values']['ses_cap_lite_password'];
    $auth_uri = $form_state['values']['ses_cap_lite_auth_uri'];
    $auth_token = ses_cap_lite_auth($username, $password, $auth_uri);
    if (!$auth_token) {
      form_set_error('ses_cap_lite_clientid', t("Error. Can't connect to Stanford CAP API. Please check your client id  and password."));
      form_set_error('ses_cap_lite_password');
    }
  }
}

/**
 * Submit handler for settings form.
 * 
 * This is based on code provided by Trellon
 */
function ses_cap_lite_settings_submit($form, &$form_state) {
  $config_vars = array(
    'ses_cap_lite_clientid',
    'ses_cap_lite_password',
    'ses_cap_lite_base_url',
    'ses_cap_lite_auth_uri',
  );
  $values = $form_state['values'];
  foreach ($config_vars as $config_var) {
    if (!empty($values[$config_var])) {
      $value = $values[$config_var];
      if (module_exists('encrypt') &&
         (strpos($config_var,'clientid') !== FALSE || strpos($config_var,'password') !== FALSE)) {
          $value = encrypt($value);
      }
      variable_set($config_var, $value);
    }

  }
  drupal_set_message(t('Configuration saved.'));
}

function ses_cap_lite_mappings($form, &$form_state) {

    // get a list of content types (and the user object) to 
    // offer as storage for CAP fields
    $bundles = field_info_bundles();
    $bundle_options = array();
    foreach ($bundles['user'] as $key => $description) {
        $bundle_options[$key] = $description['label'];
    }
    foreach ($bundles['node'] as $key => $description) {
        $bundle_options[$key] = $description['label'];
    }

    // format select list of bundles
    $form['ses_cap_lite_bundle'] = array(
        '#type' => 'select',
        '#title' => t('Content Type or User Account'),
        '#options' => $bundle_options,
        '#empty_option' => t('- Select User or content type -'),
        '#description' => t('The content type or user account object containing profile fields'),
        '#attributes' => array('class' => array('field-select')),
        '#default_value' => variable_get('ses_cap_lite_bundle','user'),
    );

    // set this hidden field so jQuery knows if the selected content type is different
    $form['ses_cap_lite_originalb'] = array(
        '#type' => 'hidden',
        '#default_value' => variable_get('ses_cap_lite_bundle','user'),
        '#attributes'=>array('id'=>'ses-cap-lite-originalb'),
    );

    // if the user changes the selected content type, we tell him to save before adding fields
    $form['cap-lite-container1'] = array(
        '#type' => 'container',
    );
    $form['cap-lite-container1']['markup'] = array(
        '#markup' => t('Click \'Save settings\' to select fields for this content type.'),
    );

    // container includes fields in the selected content type
    $form['cap-lite-container2'] = array(
        '#type' => 'container',
    );
    $form['cap-lite-container2']['markup'] = array(
        '#markup' => t('Select a field for each CAP item to receive its data. Leave a field unselected to omit that CAP item. Pick the same field for multiple items to combine them by weight order.'),
    );

    // find the available fields in the selected content type
    $bundle = variable_get('ses_cap_lite_bundle','user');
    $bundle_fields = array();
    if (!empty($bundle)) {
        if (strcmp($bundle,'user') === 0) {
            $bundle_fields = field_info_instances('user',$bundle);
        } else {
            $bundle_fields = field_info_instances('node',$bundle);
        }
    }
    // build up arrays of content type field names assorted by field type
    $form_fields = array();
    foreach ($bundle_fields as $field_name => $field_description) {
        if ($field_name === 'body') {
            $form_fields['text_long']['body'] = 'body';
        } else {
            $field_info = field_info_field($field_name);
            $form_fields[$field_info['type']][$field_name] = $field_name;
        }
    }
    // put our lists in alphabetical order
    foreach ($form_fields as $form_field_type => $form_field_array) {
        $karray = $form_field_array;
        ksort($karray);
        $form_fields[$form_field_type] = $karray;
    } 
    // build up the CAP form fields; tell the API not to flatten results array
    $form['capfields']['#tree'] = TRUE;

    // get the list of CAP fields
    module_load_include('inc','ses_cap_lite','ses_cap_lite.callback');
    $cap_fields = _ses_cap_lite_profile_fields();
    // get weighted fields for the bundle
    $weighted_fields = _ses_cap_lite_get_weighted_field_mapping($bundle);
    // add any fields that have been defined to the interface since we last saved
    $last_weight = end($weighted_fields);
    if ($last_weight === FALSE) {
        $dweight = -10;
    } else {
        $dweight = $last_weight['weight']+1;
    }
    foreach ($cap_fields as $cap_field => $cap_field_info) {
        if (!array_key_exists($cap_field,$weighted_fields)) {
            $weighted_fields[$cap_field] = array('select'=>'','weight'=>$dweight);
            $dweight += 1;
        }
    }

    // add fields to the mappings form
    foreach ($weighted_fields as $cap_field => $cap_field_info) {
        $available_fields = array();
        if (array_key_exists($cap_fields[$cap_field]['field_type'],$form_fields)) {
            $available_fields = $form_fields[$cap_fields[$cap_field]['field_type']];
        }
        $default_value = '';
        $default_weight = 0;
        if (array_key_exists($cap_field,$weighted_fields)) {
            $default_value = $weighted_fields[$cap_field]['select'];
            $default_weight = $weighted_fields[$cap_field]['weight'];
        }
        $form['capfields'][$cap_field] = array(
            'name' => array(
                '#markup' => $cap_field,
            ),
            'select' => array(
                '#type' => 'select',
                '#options' => $available_fields, //$form_fields[$cap_field_info['field_type']],
                '#empty_option' => t('- Omit -'),
                '#attributes' => array('class' => array('field-select','cap-field-select')),
		'#default_value' => $cap_field_info['select'],
            ),
            'weight' => array(
                '#type' => 'weight',
                '#title' => t('Weight'),
                '#default_value' => $cap_field_info['weight'],
                '#delta' => $dweight,
                '#title_display' => 'invisible',
            ),
        );
    }

    // allow the admin to select the text filter to use on CAP fields
    $filter_formats = filter_formats();
    $filter_options = array();
    foreach ($filter_formats as $filter_name => $filter_desc) {
        $filter_options[$filter_name] = $filter_desc->name;
    }
    $form['ses_cap_lite_filter_format'] = array(
        '#type' => 'select',
        '#title' => t('Filter Format for CAP Fields'),
        '#options' => $filter_options,
        '#description' => t('A Drupal filter format (such as Full HTML) for output of CAP fields'),
        '#attributes' => array('class' => array('field-select')),
        '#default_value' => variable_get('ses_cap_lite_filter_format',filter_fallback_format()),
        '#required' => TRUE,
    );

    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save settings'),
    );

    // attach any needed css or javascript
    $form['#attached']['js'] = array(drupal_get_path('module', 'ses_cap_lite') . '/ses_cap_lite.js');
    $form['#attached']['css'] = array(drupal_get_path('module','ses_cap_lite') . '/ses_cap_lite.css');
    return $form;
}

/**
 * Submit handler for settings form.
 */
function ses_cap_lite_mappings_submit($form, &$form_state) {
    $bundle = $form_state['values']['ses_cap_lite_bundle'];
    variable_set('ses_cap_lite_bundle',$bundle);
    if (strcmp($bundle,$form_state['values']['ses_cap_lite_originalb'])===0) {
        $mappings = variable_get('ses_cap_lite_mappings',array());
        $mappings[$bundle] = $form_state['values']['capfields'];
        variable_set('ses_cap_lite_mappings',$mappings);
    }
    variable_set('ses_cap_lite_filter_format',$form_state['values']['ses_cap_lite_filter_format']);
    drupal_set_message(t('Mappings saved.'));
}

/*
 * theme the form so that the admin can drag fields together that can share a single Drupal field
 * (for example, to write everything to the body field
 */
function theme_ses_cap_lite_mappings($variables) {

  $form = $variables['form'];

  $rows = array();
  foreach (element_children($form['capfields']) as $id) {
    $form['capfields'][$id]['weight']['#attributes']['class'] = array('cap-fields-weight');
    $rows[] = array(
      'data' => array(
        drupal_render($form['capfields'][$id]['name']),
        drupal_render($form['capfields'][$id]['select']),
        drupal_render($form['capfields'][$id]['weight']),
      ),
      'class' => array('draggable'),
    );
  }
  $output = drupal_render_children($form,array('ses_cap_lite_bundle','ses_cap_lite_originalb',
                                               'cap-lite-container1','cap-lite-container2'));
  $header = array(t('Name'), t('Select'), t('Weight'));
  $table_id = 'capfields-items-table';
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => $table_id)));
  $output .= drupal_render_children($form);

  drupal_add_tabledrag($table_id, 'order', 'sibling', 'cap-fields-weight');

  return $output;
}

/* a simple form to list the workgroups we want to manage;
 * this could be updated to include org codes
 */
function ses_cap_lite_workgroups($form, &$form_state) {

    // start with an instructive string
    $markup_str = 'Enter workgroups, one workgroup per line, to be processed for CAP profiles.';

    // set up the form
    $form['description_wrapper'] = array(
        '#type' => 'container',
    );
    $form['description_wrapper']['description'] = array(
        '#markup' => t($markup_str),
    );
    $form['ses_cap_lite_workgroups'] = array(
        '#type' => 'textarea',
        '#title' => t('Workgroups:'),
        '#default_value' => variable_get('ses_cap_lite_workgroups',''),
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save settings'),
    );

    return $form;

}

/* save our workgroup entries */
function ses_cap_lite_workgroups_submit($form, &$form_state) {
    variable_set('ses_cap_lite_workgroups',$form_state['values']['ses_cap_lite_workgroups']);
    drupal_set_message(t('Workgroups saved.'));
}

