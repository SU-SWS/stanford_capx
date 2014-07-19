<?php
/**
 * @file
 * @author [author] <[email]>
 *
 * A collection of forms
 *
 */

use CAPx\Drupal\Util\CAPx;


// /////////////////////////////////////////////////////////////////////////////
// CONNECT FORM
// /////////////////////////////////////////////////////////////////////////////


/**
 * [stanford_capx_forms_connect_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_connect_form($form, &$form_state) {

  $username = decrypt(variable_get('stanford_capx_username', ''));
  $connection = CAPx::testConnection();

   $form['auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authorization'),
    '#collapsible' => TRUE,
    '#collapsed' => $connection->status,
  );
  $form['auth']['description'] = array(
    '#markup' => t('Please enter your authentication information for the CAP API.'),
  );

  $form['auth']['stanford_capx_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID:'),
    '#default_value' => $username,
    '#required' => TRUE,
  );

  $form['auth']['stanford_capx_password'] = array(
    '#type' => 'password',
    '#title' => t('Authz secret:'),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Advanced setting for CAP API and authentication URIs'),
  );

  $form['advanced']['stanford_capx_api_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint'),
    '#description' => t('CAP API endpoint URI, only useful when switching between development/production environment.'),
    '#default_value' => variable_get('stanford_capx_api_base_url', 'https://api.stanford.edu'),
    '#required' => TRUE,
  );

  $form['advanced']['stanford_capx_api_auth_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Authentication URI'),
    '#description' => t('CAP API authentication URI.'),
    '#default_value' => variable_get('stanford_capx_api_auth_uri', 'https://authz.stanford.edu/oauth/token'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save connection settings'),
  );

  return $form;
}

/**
 * [stanford_capx_forms_connect_form_validate description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_connect_form_validate($form, &$form_state) {

  $username = $form_state['values']['stanford_capx_username'];
  $password = $form_state['values']['stanford_capx_password'];
  $endpoint = $form_state['values']['stanford_capx_api_base_url'];
  $authpoint = $form_state['values']['stanford_capx_api_auth_uri'];

  // If password changed validate auth
  if (!empty($password)) {
    $auth_test = CAPx::testAuthConnection($username, $password, $authpoint);
    if (!$auth_test->status) {
      form_set_error('stanford_capx_username', 'Could not authenticate with server. Invalid credentials. Please check your username and password');
      form_set_error('stanford_capx_password');
    }
  }

  // In case the end points change we need to get pass from db.
  $password = !empty($password) ? $password : decrypt(variable_get('stanford_capx_password', ''));

  // If authpoint changed
  if ($authpoint !== "https://authz.stanford.edu/oauth/token") {

    if (!isset($auth_test)) {
      $auth_test = CAPx::testAuthConnection($username, $password, $authpoint);
    }

    if (!$auth_test->status) {
      form_set_error("stanford_capx_api_auth_uri", "Please check your authorization url");
    }

  }

  // If endpoint changed
  if ($endpoint !== "https://api.stanford.edu") {

    if (!isset($auth_test)) {
      $auth_test = CAPx::testAuthConnection($username, $password, $authpoint);
    }

    if (isset($auth_test->token)) {
      $token = $auth_test->token;
      $end_test = CAPx::testApiConnection($token, $endpoint);
    }

  }

}

/**
 * [stanford_capx_forms_connect_form_submit description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_connect_form_submit($form, &$form_state) {

  drupal_set_message('Configuration options were saved.');

  // Username and Pass
  if (!empty($form_state['values']['stanford_capx_username'])
  && !empty($form_state['values']['stanford_capx_password'])) {

    $username = encrypt($form_state['values']['stanford_capx_username']);
    $password = encrypt($form_state['values']['stanford_capx_password']);
    variable_set('stanford_capx_username', $username);
    variable_set('stanford_capx_password', $password);

  }

  // Endpoints
  variable_set('stanford_capx_api_base_url', $form_state['values']['stanford_capx_api_base_url']);
  variable_set('stanford_capx_api_auth_uri', $form_state['values']['stanford_capx_api_auth_uri']);

}

// /////////////////////////////////////////////////////////////////////////////
// END CONNECT FORM
// /////////////////////////////////////////////////////////////////////////////


// /////////////////////////////////////////////////////////////////////////////
// MAPPER FORM
// /////////////////////////////////////////////////////////////////////////////

/**
 * [stanford_capx_mapper_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form($form, &$form_state) {

  $mapper = new stdClass();

  $form = array();
  $form += stanford_capx_mapper_form_get_machine_name_form($form, $form_state);
  $form += stanford_capx_mapper_form_get_entity_mapping_form($form, $form_state);
  $form += stanford_capx_mapper_form_get_field_mapping_form($form, $form_state);


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

/**
 * [stanford_capx_mapper_form_get_machine_name_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_get_machine_name_form($form, &$form_state) {
  $form['naming'] = array(
    '#title' => t('Mapper title'),
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#description' => t('Provide a human-readable name and machine name for this set of mapping configuration.'),
  );

  $form['naming']['title'] = array(
    '#type' => 'textfield',
    '#description' => t('Please enter a unique name for this mapper'),
    '#default_value' => isset($mapper->title) ? $mapper->title : '',
  );

  $form['naming']['machine-name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#default_value' => isset($mapper->machine_name) ? $mapper->machine_name : '',
    '#size' => 64,
    '#maxlength' => 64,
    '#description' => t('A unique name for the mapper. It must only contain lowercase letters, numbers and hyphens.'),
    '#machine_name' => array(
      'exists' => 'stanford_capx_mapper_machine_name_exits',
      'source' => array('naming','title'),
    ),
    '#disabled' => isset($mapper->machine_name),
  );

  return $form;
}

/**
 * [stanford_capx_mapper_form_get_entity_mapping_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_get_entity_mapping_form($form, &$form_state) {

  $entity_types = entity_get_info();
  $entity_options = array();
  $bundle_options = array();
  $restricted = array('capx_importer', 'capx_mapper');
  $mapper = new stdClass();

  foreach ($entity_types as $entity_name => $values) {
    if (in_array($entity_name, $restricted)) {
      continue;
    }
    $entity_options[$entity_name] = $values['label'];
    $bundle_options[$entity_name] = array();

    foreach($values['bundles'] as $bundle_machine_name => $bundle_info) {
      $bundle_options[$entity_name][$bundle_machine_name] = $bundle_info['label'];
    }

  }

  $form['entity-mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity Mapping'),
    '#description' => t('Select which entity type and bundle you would like to map CAP data into. Please select the entity type first and the bundle type will appear.'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );

  $form['entity-mapping']['entity-type'] = array(
    '#type' => 'select',
    '#description' => t(''),
    '#options' => $entity_options,
    '#default_value' => isset($mapper->entity_type) ? $mapper->entity_type : 'node',
  );

  // Build out the bundles options.
  foreach ($bundle_options as $entity_name => $bundle_opts) {

    $form['entity-mapping']['bundle-'.$entity_name] = array(
      '#type' => 'select',
      '#title' => t('Select bundle'),
      '#description' => t(''),
      '#options' => $bundle_opts,
      '#default_value' => isset($mapper->bundle) ? $mapper->bundle : 'stanford_person',
      '#states' => array(
        'visible' => array('select[name="entity-type"]' => array('value' => $entity_name)),
      ),
    );

  }

  return $form;
}


/**
 * [stanford_capx_mapper_form_get_field_mapping_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_get_field_mapping_form($form, &$form_state) {

  $entity_types = entity_get_info();

  $form['field-mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field Mapping'),
    '#description' => t(''),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );

  // **** WARNING ***** BIG HEAVY FORM ***** WARNING ******
  foreach ($entity_types as $entity_machine_name => $entity_info_values) {

    // Get all the properties of the entity type.
    $entity_info = entity_get_property_info($entity_machine_name);

    // Skip those entities with no bundles
    if (!isset($entity_info['bundles'])) {
      continue;
    }

    foreach($entity_info['bundles'] as $bundle_machine_name => $bundle_info) {
      $properties = $entity_info['properties'];
      $fields = $entity_info['bundles'][$bundle_machine_name]['properties'];

      $property_container = $entity_machine_name . "-" . $bundle_machine_name . "-properties";
      $field_container = $entity_machine_name . "-" . $bundle_machine_name . "-fields";

      // Generate property forms for each entity + bundle
      $form['field-mapping'][$property_container] = array(
        '#type' => 'fieldset',
        '#title' => t($bundle_machine_name . ' Properties'),
        '#description' => t(''),
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
        '#states' => array(
          'visible' => array(
            'select[name="entity-type"]' => array('value' => $entity_machine_name),
            'select[name="bundle-' . $entity_machine_name . '"]' => array('value' => $bundle_machine_name),
          ),
        ),
      );

      // Generate property forms for each entity + bundle
      $form['field-mapping'][$field_container] = array(
        '#type' => 'fieldset',
        '#title' => t($bundle_machine_name . ' Fields'),
        '#description' => t(''),
        '#collapsed' => TRUE,
        '#collapsible' => TRUE,
        '#states' => array(
          'visible' => array(
            'select[name="entity-type"]' => array('value' => $entity_machine_name),
            'select[name="bundle-' . $entity_machine_name . '"]' => array('value' => $bundle_machine_name),
          ),
        ),
      );


      // PROPERTIES
      // ----------
      foreach ($properties as $property_machine_name => $property_info) {
        $form['field-mapping'][$property_container][$entity_machine_name][$bundle_machine_name][$property_machine_name] = array(
            '#type' => 'textfield',
            '#title' => t($property_info['label']),
            '#description' => t(@$property_info['description']),
            '#default_value' => isset($mapper->{$property_machine_name}) ?  $mapper->{$property_machine_name} : '',
        );
      }

      // FIELDS
      // ----------------

      // @todo: field columns
      foreach ($fields as $field_machine_name => $field_info) {
        $form['field-mapping'][$field_container][$entity_machine_name][$bundle_machine_name][$field_machine_name] = array(
            '#type' => 'textfield',
            '#title' => t($field_info['label']),
            '#description' => t($field_info['description']),
            '#default_value' => isset($mapper->{$field_machine_name}) ?  $mapper->{$field_machine_name} : '',
        );
      }


    }
  }

  return $form;
}

/**
 * [stanford_capx_mapper_machine_name_exits description]
 * @param  [type] $name [description]
 * @return [type]       [description]
 */
function stanford_capx_mapper_machine_name_exits($name) {

  return FALSE;
}


/**
 * [stanford_capx_mapper_form_validate description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_validate($form, $form_state) {

}

/**
 * [stanford_capx_mapper_form_submit description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_submit($form, $form_state) {

}




