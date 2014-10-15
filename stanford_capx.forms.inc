<?php
/**
 * @file
 * @author [author] <[email]>
 *
 * A collection of forms
 *
 */

use CAPx\Drupal\Util\CAPx;
use CAPx\Drupal\Util\CAPxConnection;
use CAPx\Drupal\Util\CAPxMapper;
use CAPx\Drupal\Util\CAPxImporter;
use \Peekmo\JsonPath\JsonStore as JsonParser;

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

  $username = CAPx::getAuthUsername();
  $connection = CAPxConnection::testConnection();

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
    '#default_value' => CAPx::getAPIEndpoint(),
    '#required' => TRUE,
  );

  $form['advanced']['stanford_capx_api_auth_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Authentication URI'),
    '#description' => t('CAP API authentication URI.'),
    '#default_value' => CAPx::getAuthEndpoint(),
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
    $auth_test = CAPxConnection::testAuthConnection($username, $password, $authpoint);
    if (!$auth_test->status) {
      form_set_error('stanford_capx_username', 'Could not authenticate with server. Invalid credentials. Please check your username and password');
      form_set_error('stanford_capx_password');
    }
  }

  // In case the end points change we need to get pass from db.
  $password = !empty($password) ? $password : CAPx::getAuthPassword();

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
      $auth_test = CAPxConnection::testAuthConnection($username, $password, $authpoint);
    }

    if (isset($auth_test->token)) {
      $token = $auth_test->token;
      $end_test = CAPxConnection::testApiConnection($token, $endpoint);
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
// MAPPER FORMS
// /////////////////////////////////////////////////////////////////////////////

/**
 * [stanford_capx_mapper_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form($form, &$form_state) {

  $form = array();
  $form += stanford_capx_mapper_form_get_machine_name_form($form, $form_state);
  $form += stanford_capx_mapper_form_get_entity_mapping_form($form, $form_state);
  $form += stanford_capx_mapper_form_get_field_mapping_form($form, $form_state);

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save mapper',
  );

  $form['actions']['cancel'] = array(
    '#markup' => l(t('Cancel'), 'admin/config/capx/mapper'),
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

  // Globally loaded mapper.
  global $_capx_mapper;
  $mapper = $_capx_mapper;

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
    '#disabled' => isset($mapper->machine_name),
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

  $entity_types = capx_entity_get_info();
  $entity_options = array();
  $bundle_options = array();
  global $_capx_mapper;
  $mapper = $_capx_mapper;

  foreach ($entity_types as $entity_name => $values) {
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
    '#disabled' => isset($mapper->machine_name),
  );

  // Build out the bundles options.
  foreach ($bundle_options as $entity_name => $bundle_opts) {

    $form['entity-mapping']['bundle-'.$entity_name] = array(
      '#type' => 'select',
      '#title' => t('Select bundle'),
      '#description' => t(''),
      '#options' => $bundle_opts,
      '#default_value' => isset($mapper->bundle_type) ? $mapper->bundle_type : 'stanford_person',
      '#states' => array(
        'visible' => array('select[name="entity-type"]' => array('value' => $entity_name)),
      ),
      '#disabled' => isset($mapper->machine_name),
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

  $entity_types = capx_entity_get_info();
  $field_collections = array();
  global $_capx_mapper;
  $mapper = $_capx_mapper;
  global $CAP_RESTRICTED_FIELD_PROPERTIES;

  $form['field-mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field Mapping'),
    '#description' => t(''),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );

  $form['field-mapping']['help']['#markup'] = "<p>" . t('Use the data browser to find the information you would like to map into the various different properties and fields below. You will need to use JSONPath syntax. For more information on JSONPath please read the documentation. ') . l('JSONPath Documentation →', 'http://goessner.net/articles/JsonPath/', array('attributes' => array('target' => '_blank'))) . "</p>";


  // ******************************************************
  // **** WARNING ***** BIG HEAVY FORM ***** WARNING ******
  // ******************************************************

  // PROPERTIES
  // ---------------------------------------------------------------------
  foreach ($entity_types as $entity_machine_name => $entity_info_values) {

    // If this is an edit mapper form (mapper exists) do not render the other
    // entities or bundles
    if (isset($mapper->settings['entity_type']) && $mapper->settings['entity_type'] !== $entity_machine_name) {
      continue;
    }

    // Get all the properties of the entity type.
    $entity_info = entity_get_property_info($entity_machine_name);

    $property_container = $entity_machine_name . "-properties";
    // Generate property forms for each entity + bundle
    $form['field-mapping'][$property_container] = array(
      '#type' => 'fieldset',
      '#title' => t($entity_machine_name . ' Properties'),
      '#description' => t(''),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          'select[name="entity-type"]' => array('value' => $entity_machine_name),
        ),
      ),
    );

    $properties = $entity_info['properties'];
    $properties = stanford_capx_filter_properties($entity_machine_name, $properties);

    foreach ($properties as $property_machine_name => $prop_info) {

      $form['field-mapping'][$property_container][$property_machine_name] = array(
          '#type' => 'textfield',
          '#title' => t($prop_info['label']),
          '#description' => t($prop_info['description']),
          '#default_value' => isset($mapper->properties[$property_machine_name]) ? $mapper->properties[$property_machine_name] : '',
          '#theme_wrappers' => array('stanford_capx_columns_to_the_right'),
          '#field_info' => $prop_info,
      );
    }

  }

  // FIELDS
  // ---------------------------------------------------------------------
  foreach ($entity_types as $entity_machine_name => $entity_info_values) {

    // If this is an edit mapper form (mapper exists) do not render the other
    // entities or bundles
    if (isset($mapper->settings['entity_type']) && $mapper->settings['entity_type'] !== $entity_machine_name) {
      continue;
    }

    // Get all the properties of the entity type.
    $entity_info = entity_get_property_info($entity_machine_name);

    // Skip those entities with no bundles
    if (!isset($entity_info['bundles'])) {
      continue;
    }

    foreach($entity_info['bundles'] as $bundle_machine_name => $bundle_info) {

      // Skip bundles that dont match a loaded mapper.
      if (isset($mapper->settings['bundle_type']) && $bundle_machine_name !== $mapper->settings['bundle_type']) {
        continue;
      }

      $fields = $entity_info['bundles'][$bundle_machine_name]['properties'];
      $field_container = $entity_machine_name . "-" . $bundle_machine_name . "-fields";

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

      foreach ($fields as $field_machine_name => $field_info) {

        // Field collections need their own fieldset and fields.
        if (strpos($field_info['type'], 'field_collection_item') !== FALSE) {
          $field_collections[$entity_machine_name][$bundle_machine_name][$field_machine_name] = $field_info;
          continue;
        }

        $field_info_field = field_info_field($field_machine_name);
        $field_instance_info = field_info_instance($entity_machine_name, $field_machine_name, $bundle_machine_name);

        $form['field-mapping'][$field_container][$field_machine_name] = array(
          '#type' => 'container',
          '#title' => t($field_instance_info['label']),
          '#description' => t($field_instance_info['description']),
          '#field_info' => $field_info_field,
          '#field_instance_info' => $field_instance_info,
          '#theme_wrappers' => array('stanford_capx_columns_to_the_right'),
        );

        // Do not give all options for the file field. File field processor
        // will take care of most.
        if ($field_info_field['type'] == "file" || $field_info_field['type'] == "image") {
          $form['field-mapping'][$field_container][$field_machine_name][0] = array(
            '#type' => 'textfield',
            '#title' => t('File Information'),
            '#description' => t('In your JSON Path selector please pass in the whole array of image information. eg: $.profilePhotos.bigger'),
            '#default_value' => @$mapper->fields[$field_machine_name][0],
          );

          continue;
        }

        // Relation module doesn't have this column key set.
        if (!isset($field_info_field['columns'])) {
          continue;
        }
        foreach ($field_info_field['columns'] as $column_key => $column_info) {
          if (!in_array($column_key, $CAP_RESTRICTED_FIELD_PROPERTIES)) {
            $form['field-mapping'][$field_container][$field_machine_name][$column_key] = array(
              '#type' => 'textfield',
              '#title' => t($column_key),
              '#description' => t(@$column_info['description']),
              '#default_value' => @$mapper->fields[$field_machine_name][$column_key],
            );
          }
        }

      }

    }
  } // end field looop

  // FIELD COLLECTIONS
  // ----------------------------------------------------------------------

  foreach ($field_collections as $entity_machine_name => $bundles) {
    foreach ($bundles as $bundle_machine_name => $fields) {
      foreach ($fields as $field_machine_name => $field_info) {

        $instances = field_info_instances('field_collection_item', $field_machine_name);
        $collection_container = $entity_machine_name . "-" . $bundle_machine_name . "-collections";

        // Fieldset for all fields on this entity -> bundle -> collection field
        $form['field-mapping'][$collection_container] = array(
          '#type' => 'fieldset',
          '#title' => t($field_machine_name . ' Field Collection'),
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

        foreach ($instances as $fc_field_name => $instance_info) {

          $fc_field_info = field_info_field($fc_field_name);

          $form['field-mapping'][$collection_container][$field_machine_name][$fc_field_name] = array(
            '#type' => 'container',
            '#title' => t($instance_info['label']),
            '#description' => t($instance_info['description']),
            '#theme_wrappers' => array('stanford_capx_columns_to_the_right'),
            '#field_info' => $fc_field_info,
            '#field_instance_info' => $instance_info,
          );

          // Do not give all options for the file field. File field processor
          // will take care of most.
          if ($fc_field_info['type'] == "file" || $fc_field_info['type'] == "image") {
            $form['field-mapping'][$collection_container][$field_machine_name][$fc_field_name][0] = array(
              '#type' => 'textfield',
              '#title' => t('File Info'),
              '#description' => t('In your JSON Path selector please pass in the whole array of image information. eg: $.profilePhotos.bigger'),
              '#default_value' => @$mapper->collections[$field_machine_name][$fc_field_name][0],
            );
            continue;
          }

          foreach ($fc_field_info['columns'] as $column_key => $column_info) {
            if (!in_array($column_key, $CAP_RESTRICTED_FIELD_PROPERTIES)) {
              $form['field-mapping'][$collection_container][$field_machine_name][$fc_field_name][$column_key] = array(
                '#type' => 'textfield',
                '#title' => t($column_key),
                '#description' => t(@$column_info['description']),
                '#default_value' => @$mapper->collections[$field_machine_name][$fc_field_name][$column_key],
              );
            }
          }
        }

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

  $result = db_select('capx_cfe', 'cfe')
              ->fields('cfe', array('machine_name'))
              ->condition('machine_name', $name)
              ->condition('type', 'mapper')
              ->execute();
  $count = $result->rowCount();

  // We found a match!
  if ($count > 0) {
    return TRUE;
  }

  // Nothing by the passed in name.
  return FALSE;
}

/**
 * Takes in an array of properties and filters out the ones that users should
 * never map to. Things like nid, vid, comments, etc.
 * @param  array  $properties [description]
 * @return [type]             [description]
 */
function stanford_capx_filter_properties($entity_type, $properties = array()) {

  $blacklist = array();
  $property_keys = array_keys($properties);

  // Blocks / Beans
  $blacklist['bean'][] = "bid";
  $blacklist['bean'][] = "vid";
  $blacklist['bean'][] = "delta";
  $blacklist['bean'][] = "view_mode";
  $blacklist['bean'][] = "type";
  $blacklist['bean'][] = "url";
  $blacklist['bean'][] = "uid";

  // Field collection item.
  $blacklist['field_collection_item'][] = "item_id";
  $blacklist['field_collection_item'][] = "revision_id";
  $blacklist['field_collection_item'][] = "field_name";
  $blacklist['field_collection_item'][] = "archived";
  $blacklist['field_collection_item'][] = "url";
  $blacklist['field_collection_item'][] = "host_entity";

  // Node
  $blacklist['node'][] = "nid";
  $blacklist['node'][] = "vid";
  $blacklist['node'][] = "is_new";
  $blacklist['node'][] = "changed";
  $blacklist['node'][] = "created";
  $blacklist['node'][] = "type";
  $blacklist['node'][] = "language";
  $blacklist['node'][] = "url";
  $blacklist['node'][] = "edit_url";
  $blacklist['node'][] = "status";
  $blacklist['node'][] = "promote";
  $blacklist['node'][] = "sticky";
  $blacklist['node'][] = "author";
  $blacklist['node'][] = "source";
  $blacklist['node'][] = "revision";
  $blacklist['node'][] = "comment";
  $blacklist['node'][] = "comments";
  $blacklist['node'][] = "comment_count";
  $blacklist['node'][] = "comment_count_new";
  $blacklist['node'][] = "body"; // body is not a property!

  // Terms
  $blacklist['taxonomy_term'][] = "tid";
  $blacklist['taxonomy_term'][] = "weight";
  $blacklist['taxonomy_term'][] = "node_count";
  $blacklist['taxonomy_term'][] = "url";
  $blacklist['taxonomy_term'][] = "vocabulary";
  $blacklist['taxonomy_term'][] = "parent";
  $blacklist['taxonomy_term'][] = "parents_all";

  // User
  $blacklist['user'][] = "uid";
  $blacklist['user'][] = "url";
  $blacklist['user'][] = "edit_url";
  $blacklist['user'][] = "last_access";
  $blacklist['user'][] = "last_login";
  $blacklist['user'][] = "roles";
  $blacklist['user'][] = "status";
  $blacklist['user'][] = "theme";

  // Anything we don't know about
  $blacklist['unknown'][] = "id";
  $blacklist['unknown'][] = "vid";
  $blacklist['unknown'][] = "url";
  $blacklist['unknown'][] = "edit_url";
  $blacklist['unknown'][] = "uid";

  if (!isset($blacklist[$entity_type])) {
    $entity_type = "unknown";
  }

  // Rip out the blacklisted items.
  $allowed = array_diff($property_keys, $blacklist[$entity_type]);
  $allowed = array_flip($allowed);
  $properties = array_intersect_key($properties, $allowed);

  return $properties;
}

/**
 * Validation function for the mapper form.
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_validate($form, $form_state) {
  // Recursively validate json selectors for mapped fields and properties.

  // Only validate the entity/bundle that was selected...
  $entity = $form_state['values']['entity-type'];
  $bundle = $form_state['values']['bundle-' . $entity];

  // Right now all entities have fields and properties.
  stanford_capx_mapper_form_validate_element($form_state['values']['field-mapping'][$entity . "-properties"], array('field-mapping'));
  stanford_capx_mapper_form_validate_element($form_state['values']['field-mapping'][$entity . "-" . $bundle . "-fields"], array('field-mapping'));

  // Only some have field collections.
  if (isset($form_state['values']['field-mapping'][$entity . "-" . $bundle .  "-collections"])) {
    stanford_capx_mapper_form_validate_element($form_state['values']['field-mapping'][$entity . "-" . $bundle . "-collections"], array('field-mapping'));
  }

  // Set the error messages after everything has been looped through.
  stanford_capx_mapper_form_validate_element_set_messages();
}

/**
 * Validate the user input from the mapper form by checking to see if the
 * selector finds something in the API schema.
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_validate_element($element, $parents = array()) {
  static $json_parser;
  $form_error = &drupal_static(__FUNCTION__ . "_error", FALSE);
  $form_field_warnings = &drupal_static(__FUNCTION__ . "_warnings", FALSE);

  // Load and cache the parser.
  if (!isset($json_parser)) {
    $schema = stanford_capx_schema_format_validation();
    $json_parser = new JsonParser($schema);
  }

  // Recursively iterate over child elements.
  foreach ($element as $name => $child_element) {
    array_push($parents, $name);
    if (is_array($child_element)) {
      stanford_capx_mapper_form_validate_element($child_element, $parents);
    }
    elseif (is_string($child_element) && strlen(trim($child_element))) {
      try {
        // Data types: 'string', 'boolean', 'integer', 'array' or 'object'.
        $data_type = $json_parser->get($child_element);

        // Usually an empty array is returned for an invalid selector.
        if (is_array($data_type) && !is_string(array_pop($data_type))) {
          throw new \Exception('String indicates a valid element.');
        }
      }
      catch (\Exception $e) {
        // Catch any Exception from JsonParser or the condition above.
        // @TODO: Enable validation when the schema is consistent with the data.
        // form_set_error(implode('][', $parents));

        if ($name == "title") {
          $field_name = "title";
        }
        else {
          $field_name = $parents[(count($parents) - 2)];
        }

        // Record the problematic selector for display in a warning message.
        $form_field_warnings[] = "<b>" . $field_name . "</b> has a selector of  <b>" . check_plain($child_element) . "</b> that does not match any element in the API.";
        $form_error = TRUE;
      }
    }
    array_pop($parents);
  }

}

/**
 * Sets the message for the element validation.
 */
function stanford_capx_mapper_form_validate_element_set_messages() {
  $form_error = &drupal_static("stanford_capx_mapper_form_validate_element_error", FALSE);
  $form_field_warnings = &drupal_static("stanford_capx_mapper_form_validate_element_warnings", FALSE);

  // Set the error message once when the recursive calls have finished.
  if ($form_error) {
    $field_warnings_list = theme('item_list', array(
      'title' => '',
      'items' => $form_field_warnings,
      'type' => 'ul',
      'attributes' => array()
    ));
    $browser_link = l(t('Data Browser'),
      'admin/config/capx/data-browser', array(
        'attributes' => array('target' => '_blank')
      ));

    drupal_set_message(
      t('The following selectors did not match an element in the profile schema. Try using the !databrowser to find a valid selector.!warnings',
        array(
          '!warnings' => $field_warnings_list,
          '!databrowser' => $browser_link,
        )
      ), 'warning');
  }
}

/**
 * Submit handler for the mapper form. Create and save the mapper entity.
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_mapper_form_submit($form, $form_state) {

  $values = $form_state['values'];
  $mapper = capx_cfe_load_by_machine_name($values['machine-name'], 'mapper');

  if (!$mapper) {
    $mapper = entity_create('capx_cfe', array());
  }

  $mapper->title = $values['title'];
  $mapper->machine_name = $values['machine-name'];
  $mapper->type = "mapper";

  $settings = array();
  $settings['fields'] = array();
  $settings['properties'] = array();
  $settings['collections'] = array();

  $entity = $values['entity-type'];
  $bundle = $values['bundle-' . $entity];

  $settings['entity_type'] = $entity;
  $settings['bundle_type'] = $bundle;

  // If there are properties...
  $properties = @$values['field-mapping'][$entity . "-properties"];
  if (!empty($properties)) {
    $settings['properties'] = $properties;
  }

  // If there are fields...
  $fields = @$values['field-mapping'][$entity . "-" . $bundle . "-fields"];
  if (!empty($fields)) {
    $settings['fields'] = $fields;
  }

  // If there are collections...
  $collections = @$values['field-mapping'][$entity . "-" . $bundle . "-collections"];
  if (!empty($collections)) {
    $settings['collections'] = $collections;
  }

  $mapper->settings = $settings;

  // New vs Edit
  if (!empty($mapper->is_new)) {
    drupal_set_message('Congratulations, Your mapper has been created.', 'status', FALSE);
  }
  else {
    drupal_set_message("Your mapper has been updated.", 'status', FALSE);
  }

  // Do the save
  capx_mapper_save($mapper);

  // Are we sure we want this?
  drupal_goto('admin/config/capx/mapper');
}

/**
 * Delete the mapper by first prompting for a confirmation form.
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_admin_config_mapper_delete($form, $form_state, $machine_name) {
  $form = array();
  $mapper = capx_cfe_load_by_machine_name($machine_name, 'mapper');

  if (!$mapper) {
    throw new Exception("Could not load a mapper with provided machine name. Please check url.");
  }

  $importers = CAPxImporter::loadImportersByMapper($mapper);
  $items = array();

  foreach ($importers as $importer) {
    $items[] = l($importer->title, 'admin/config/capx/importer/edit/' . $importer->identifier());
  }

  if (count($items)) {
    $list = array(
      'title' => 'This mapper is being used by these importers:',
      'items' => $items,
    );
    $form['importers'] = array(
      '#markup' => theme('item_list', $list),
    );
  }

  $form['machine-name'] = array(
    '#type' => 'hidden',
    '#value' => $mapper->machine_name,
  );
  $question = "Are you sure you want to delete " . $mapper->title . "?";
  $path = "admin/config/capx/mapper";
  $description = t('This action cannot be undone.');
  $name = 'confirm_mapper_delete';
  $form = confirm_form($form, $question, $path, $description, 'Yes, please delete', 'Cancel', $name);

  if (count($items)) {
    $form['actions']['#disabled'] = 'disabled';
  }

  return $form;
}

/**
 * Delete the mapper.
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_admin_config_mapper_delete_submit($form, $form_state) {
  $mapper = capx_cfe_load_by_machine_name($form_state['values']['machine-name'], 'mapper');
  capx_cfe_delete($mapper);
  drupal_set_message('Mapper was deleted.', 'status');
  drupal_goto('admin/config/capx/mapper');
}


// /////////////////////////////////////////////////////////////////////////////
// IMPORTER FORMS
// /////////////////////////////////////////////////////////////////////////////


/**
 * [stanford_capx_importer_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_importer_form($form, &$form_state) {

  global $_capx_importer;
  $importer = $_capx_importer;

  // Do a check to see if the organization codes are available. If not, prompt
  // the user to update them.
  $has_orgs = \CAPx\Drupal\Organizations\Orgs::checkOrgs();
  if (!$has_orgs) {
    drupal_set_message('The organization codes are not available. ' . l('Sync with the API now.', 'admin/config/capx/organizations/sync', array('query' => array('destination' => current_path()))), 'warning');
  }

  // Configuration group
  // -–-–-–-–-–-–-–-–-–-–-

  $form['naming'] = array(
    '#title' => t('Configuration'),
    '#type' => 'fieldset',
    '#collapsed' => isset($importer),
    '#collapsible' => isset($importer),
    '#description' => t('Provide a human-readable name and machine name for this selection of profiles.'),
  );

  $form['naming']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Importer name'),
    '#description' => t('Please enter a unique name for this Importer'),
    '#default_value' => isset($importer->title) ? $importer->title : '',
    '#disabled' => isset($importer->machine_name),
  );

  $form['naming']['machine-name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#default_value' => isset($importer->machine_name) ? $importer->machine_name : '',
    '#size' => 64,
    '#maxlength' => 64,
    '#description' => t('A unique name for the importer. It must only contain lowercase letters, numbers and hyphens.'),
    '#machine_name' => array(
      'exists' => 'stanford_capx_importer_machine_name_exits',
      'source' => array('naming','title'),
    ),
    '#disabled' => isset($importer->machine_name),
  );

  // Create options...
  $mapping_options = array();
  $mappers = CAPxMapper::loadAllMappers();
  foreach ($mappers as $cfid => $mapper) {
    $mapping_options[$mapper->machine_name] = $mapper->title;
  }

  $form['naming']['mapper'] = array(
    '#type' => 'select',
    '#title' => t('Mapping'),
    '#description' => t('Choose the Mapper you would like to import these profiles with.'),
    '#options' => $mapping_options,
    '#default_value' => isset($importer->mapper) ? $importer->mapper : '',
    // '#disabled' => isset($importer->mapper),
  );

  // Cron Settings
  // -–-–-–-–-–-–-–-–-–-–-

  $form['cron'] = array(
    '#title' => t('Sync options'),
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );

  $form['cron']['cron_option'] = array(
    '#title' => t('Sync'),
    '#description' => t('Select an interval on how often this importer should syncronize with the CAP server. This setting is dependant on !cron.', array('!cron' => l('cron configuration', 'admin/config/system/cron'))),
    '#type' => 'select',
    '#options' => CAPxImporter::getCronOptions(),
    '#default_value' => isset($importer->cron_option) ? $importer->cron_option : 'daily',
  );

  // Profile group
  // -–-–-–-–-–-–-–-–-–-–-

  $form['groups'] = array(
    '#title' => t('Add groups'),
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#description' => t(''),
  );

  $link = l(t('Stanford University Org Code Hierarchy'), 'http://www.stanford.edu/dept/pres-provost/budget/org/orgchart/');

  $form['groups']['organization'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization(s)'),
    '#description' => t('Please select the name of the organization you wish to import. Learn more about !orgchart.', array('!orgchart' => $link)),
    '#default_value' => isset($importer->organization) ? $importer->organization : '',
    '#autocomplete_path' => 'admin/config/capx/autocomplete/orgs', // @todo: this.
  );

  $form['groups']['child_orgs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Include child organizations'),
    '#description' => t('Enable it to retrieve all the members from child organizations.'),
    '#default_value' => isset($importer->child_orgs) ? $importer->child_orgs : '',
  );

  $link = l(t('workgroup manager website'), 'https://workgroup.stanford.edu');
  $form['groups']['workgroup'] = array(
    '#type' => 'textfield',
    '#title' => t('Workgroup'),
    '#description' => t('Enter the name of the workgroup you wish to import. You can learn more about workgroups at Stanford, and get propernames for import, at the !managerlink.', array('!managerlink' => $link)),
    '#default_value' => isset($importer->workgroup) ? $importer->workgroup : '',
  );

  $form['individuals'] = array(
    '#title' => t('Add individuals'),
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#description' => t(''),
  );

  $form['individuals']['sunet_id'] = array(
    '#type' => 'textarea',
    '#title' => t('SUNet IDs'),
    '#description' => t('Enter SUNet IDs for the profiles you wish to import. Enter multiple SUNet IDs by separating them with a comma.'),
    '#default_value' => isset($importer->sunet_id) ? $importer->sunet_id : '',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['actions']['saveandimport'] = array(
    '#type' => 'button',
    '#name' => 'saveandimport',
    '#submit' => array(
      'stanford_capx_importer_form_submit',
      'stanford_capx_importer_form_submit_run_import',
    ),
    '#executes_submit_callback' => TRUE,
    '#value' => t('Save & Import Now'),
  );

  $form['#redirect'] = "admin/config/capx/importer";

  return $form;
}

/**
 * [stanford_capx_importer_machine_name_exits description]
 * @todo refactor this with the mapper_machine_name_exists function.
 * @param  [type] $name [description]
 * @return [type]       [description]
 */
function stanford_capx_importer_machine_name_exits($name) {

  $result = db_select('capx_cfe', 'cfe')
              ->fields('cfe', array('machine_name'))
              ->condition('machine_name', $name)
              ->condition('type', 'importer')
              ->execute();
  $count = $result->rowCount();

  // We found a match!
  if ($count > 0) {
    return TRUE;
  }

  // Nothing by the passed in name.
  return FALSE;
}

/**
 * [stanford_capx_importer_form_validate description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_importer_form_validate($form, &$form_state) {
  // @todo: validate options
}

/**
 * [stanford_capx_importer_form_submit description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_importer_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $importer = CAPxImporter::loadImporter($values['machine-name']);

  if (!$importer) {
    $importer = entity_create('capx_cfe', array('type' => 'importer'));
  }

  $importer->title = $values['title'];
  $importer->machine_name = $values['machine-name'];
  $meta = $importer->getMeta();
  $importer->setMeta($meta);
  $settings = array();

  // Put settings in here!
  $settings['mapper'] = $values['mapper'];
  $settings['organization'] = $values['organization'];
  $settings['child_orgs'] = $values['child_orgs'];
  $settings['workgroup'] = $values['workgroup'];
  $settings['sunet_id'] = $values['sunet_id'];
  $settings['cron_option'] = $values['cron_option'];

  $importer->settings = $settings;

  // New vs Edit
  if (!empty($importer->is_new)) {
    drupal_set_message('Congratulations, Your importer has been created.', 'status', FALSE);
  }
  else {
    drupal_set_message("Your importer has been updated.", 'status', FALSE);
  }

  // Do the save
  capx_importer_save($importer);

}

/**
 * Submit callback function that runs the batch api import for this importer
 * @return [type] [description]
 */
function stanford_capx_importer_form_submit_run_import($form, $form_state) {
  $values = $form_state['values'];
  $importer = CAPxImporter::loadEntityImporter($values['machine-name']);

  if (!$importer) {
    throw new \Exception("Could not load importer by machine name");
  }

  $importer->createBatch();

  drupal_set_message('Profiles successfully imported', 'status');
  batch_process($form['#redirect']);

}

/**
 * [stanford_capx_admin_config_importer_delete description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_admin_config_importer_delete($form, $form_state, $machine_name) {
  $form = array();
  $importer = CAPxImporter::loadImporter($machine_name);

  if (!$importer) {
    throw new Exception("Could not load an importer with provided machine name. Please check url.");
  }

  $form['machine-name'] = array(
    '#type' => 'hidden',
    '#value' => $importer->machine_name,
  );

  $question = "Are you sure you want to delete " . $importer->title . "?";
  $path = "admin/config/capx/importer";
  $description = t('This action cannot be undone.');
  $name = 'confirm_importer_delete';
  $form = confirm_form($form, $question, $path, $description, 'Yes, please delete', 'Cancel', $name);
  return $form;
}

/**
 * Delete the importer.
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_admin_config_importer_delete_submit($form, $form_state) {
  $importer = CAPxImporter::loadImporter($form_state['values']['machine-name']);
  capx_cfe_delete($importer);
  drupal_set_message('Importer was deleted.', 'status');
  drupal_goto('admin/config/capx/importer');
}

// /////////////////////////////////////////////////////////////////////////////
// FORM ALTERS
// /////////////////////////////////////////////////////////////////////////////

/**
 * Implements hook_form_alter().
 */
function stanford_capx_form_alter(&$form, &$form_state, $form_id) {

  // If on an entity form add the vertical tab and disable fields that are
  // mapped.
  if (!empty($form['#entity_type'])) {

    if ($form['#entity_type'] == "user") {
      $entity = $form["#user"];
    }
    else {
      $entity = $form["#entity"];
    }

    if (isset($entity->capx)) {
      stanford_capx_entity_form_add_vt($form, $form_state, $form_id, $entity);
      stanford_capx_entity_form_disable_mapped_fields($form, $form_state, $form_id);
    }
  }

}

/**
 * [stanford_capx_entity_form_add_vt description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @param  [type] $form_id    [description]
 * @return [type]             [description]
 */
function stanford_capx_entity_form_add_vt(&$form, &$form_state, $form_id, $entity) {
   // Create a fieldset that will be included in the vertical tab.
  $form['capx_vertical_tab'] = array(
    '#type' => 'fieldset',
    '#title' => t('CAPx Information'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#weight' => -99,
    '#description' => t('CAPx information about this vertical tab goes here'),
    '#summary' => t('summary'),
    // The #group value must match the name of the vertical tabs element.
    // In most cases, this is 'additional_settings'.
    '#group' => 'additional_settings',
    // '#attached' => array(
    //   'js' => array(
    //     'vertical-tabs' => drupal_get_path('module', 'vertical_tabs_example') . '/vertical_tabs_example.js',
    //   ),
    // ),
  );

  $capx_info = $entity->capx;

  // Remove a few things.
  unset($capx_info['entity_type']);
  unset($capx_info['bundle_type']);
  unset($capx_info['id']);
  unset($capx_info['entity_id']);

  // Small tweak
  $capx_info['importer'] = l($capx_info['importer'], 'admin/config/capx/importer/edit/' . $capx_info['importer']);

  $header = array_keys($capx_info);
  $rows = array_values($capx_info);

  $vars = array(
    'header' => $header,
    'rows' => array('data' => $rows),
    'attributes' => array(),
    'caption' => '',
    'colgroups' => array(),
    'sticky' => '',
    'empty' => '',
  );

  $table = theme_table($vars);

  $form['capx_vertical_tab']['tmpinfo']['#markup'] = $table;

}

/**
 * Disable all fields that have mapped data from all users.
 * @todo all of this. We may have to change the way we look up fields as they
 * might not be in the order we are assuming below.
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @param  [type] $ford_id    [description]
 * @return [type]             [description]
 */
function stanford_capx_entity_form_disable_mapped_fields(&$form, &$form_state, $ford_id) {

  $entity = $form["#entity"];
  $importer = CAPxImporter::loadImporter($entity->capx['importer']);
  $mapper = CAPxMapper::loadMapper($importer->mapper);
  global $language;
  $lang = $language->language;

  $fields = $mapper->fields;
  $properties = $mapper->properties;
  $collections = $mapper->collections;

  $description = t('This field is mapped with CAP data and cannot be changed through the UI. Please visit the CAP website to update this field.');

  // Disable any properties...
  foreach ($properties as $property_name => $value) {
    if (!empty($value)) {
      $form[$property_name]['#disabled'] = TRUE;
      $form[$property_name]['#description'] = $description;
    }
  }

  // Disable any fields with data...
  foreach ($fields as $field_name => $columns) {
    foreach ($columns as $column_name => $value) {
      if (!empty($value)) {
        $form[$field_name]["#disabled"] = TRUE;
        $form[$field_name][LANGUAGE_NONE]['#description'] =
        $form[$field_name][LANGUAGE_NONE][0]['#description'] = $description;
        break;
      }
    }
  }

  // Disable any field collection fields with data.
  foreach ($collections as $field_name => $fc_field) {
    foreach ($fc_field as $fc_field_name => $value) {

        if (!empty($value)) {
          foreach ($form[$field_name][LANGUAGE_NONE] as $key => $fc_entity) {
            if (!is_numeric($key)) {
              continue;
            }
            $form[$field_name][LANGUAGE_NONE][$key][$fc_field_name]["#disabled"] = TRUE;
            $form[$field_name][LANGUAGE_NONE][$key][$fc_field_name][LANGUAGE_NONE][0]['#description'] = $description;
           }
        }

    }
  }


}

/**
 * Recursively callled function that provides an array of information that is
 * used in the mapper form validation callback.
 * @see stanford_capx_mapper_form_validate_element()
 * @param [type] [description]
 *
 * @return [type] [description]
 */
function stanford_capx_schema_format_validation($schema = NULL) {
  $formatted_schema = array();

  // Load the schema when not making a recursive function call (the first call).
  if (empty($schema)) {
    $schema = stanford_capx_schema_load('array');
  }

  // Iterate through the schema elements according to element type.
  if (isset($schema['type'])) {
    // @TODO: Remove isset() when the schema is fully documented.
    if ($schema['type'] == 'object' && isset($schema['properties'])) {
      foreach ($schema['properties'] as $name => $property) {
        $formatted_schema[$name] = stanford_capx_schema_format_validation($property);
      }
    }
    elseif ($schema['type'] == 'array') {
      $formatted_schema = array(stanford_capx_schema_format_validation($schema['items']));
    }
    else {
      // Leaf element set to a string explaining the data type. E.g. 'integer'.
      $formatted_schema = check_plain($schema['type']);
    }
  }

  return $formatted_schema;
}


/**
 * The global configuration settings form for CAPx.
 *
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_config_settings($form, &$form_state) {

  $form['#tree'] = TRUE;
  $form['variables']['stanford_capx_batch_limit'] = array(
    '#title' => t('Batch, or cron, processing limit'),
    '#description' => t('The number of profiles to process on one sync operation.'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get("stanford_capx_batch_limit", 100),
  );

  $form['variables']['stanford_capx_orphan_action'] = array(
    '#title' => t('Action to perform on orphaned profiles'),
    '#description' => t('What should happen to a profile that is marked as orphaned?'),
    '#type' => "select",
    '#options' => array(
      'unpublish' => t('Unpublish'),
      'delete' => t('Delete'),
    ),
    '#default_value' => variable_get('stanford_capx_orphan_action', 'unpublish'),
  );

  $form['submit'] = array(
    "#type" => 'submit',
    "#name" => 'submit',
    "#value" => "Submit",
  );

  return $form;
}

/**
 * [stanford_capx_forms_config_settings_form_submit description]
 * @param  [type] $form        [description]
 * @param  [type] $form_values [description]
 * @return [type]              [description]
 */
function stanford_capx_forms_config_settings_submit($form, $form_values) {
  $values = $form_values['values'];

  // Loop through the variables tree and set em.
  foreach ($values['variables'] as $var_name => $var_value) {
    variable_set($var_name, $var_value);
  }
}
