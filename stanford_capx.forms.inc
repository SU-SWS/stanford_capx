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
 * Connection details form builder.
 */
function stanford_capx_forms_connect_form($form, &$form_state) {

  $username = CAPx::getAuthUsername();
  $connection = CAPxConnection::testConnection();

  $form['status'] = array(
    '#type' => 'fieldset',
    '#title' => t('Connection Status'),
    '#collapsible' => TRUE,
  );
  $form['status']['status'] = array(
    '#markup' => stanford_capx_connection_status_block(),
  );

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
    '#title' => t('Password:'),
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

  // If password changed validate auth.
  if (!empty($password)) {
    $auth_test = CAPxConnection::testAuthConnection($username, $password, $authpoint);
    if (!$auth_test->status) {
      form_set_error('stanford_capx_username', t('Could not authenticate with server. Invalid credentials. Please check your username and password.'));
      form_set_error('stanford_capx_password');
    }
  }

  // In case the end points change we need to get pass from db.
  $password = !empty($password) ? $password : CAPx::getAuthPassword();

  // If authpoint changed.
  if ($authpoint !== "https://authz.stanford.edu/oauth/token") {

    if (!isset($auth_test)) {
      $auth_test = CAPxConnection::testAuthConnection($username, $password, $authpoint);
    }

    if (!$auth_test->status) {
      form_set_error("stanford_capx_api_auth_uri", t("Please check your authorization url."));
    }

  }

  // If endpoint changed.
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

  drupal_set_message(t('Configuration options were saved.'));

  // Username and Pass.
  if (!empty($form_state['values']['stanford_capx_username'])
  && !empty($form_state['values']['stanford_capx_password'])) {

    $username = encrypt($form_state['values']['stanford_capx_username']);
    $password = encrypt($form_state['values']['stanford_capx_password']);
    variable_set('stanford_capx_username', $username);
    variable_set('stanford_capx_password', $password);

  }

  // Endpoints.
  variable_set('stanford_capx_api_base_url', $form_state['values']['stanford_capx_api_base_url']);
  variable_set('stanford_capx_api_auth_uri', $form_state['values']['stanford_capx_api_auth_uri']);

  // In order to get here we have to have a valid connection. Lets get the org codes
  // and schema.
  stanford_capx_schema_refresh();
  stanford_capx_organizations_sync();
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
function stanford_capx_mapper_form($form, &$form_state, $mapper = NULL) {

  $form = array();
  $form += stanford_capx_mapper_form_get_machine_name_form($form, $form_state, $mapper);
  $form += stanford_capx_mapper_form_get_entity_mapping_form($form, $form_state, $mapper);
  $form += stanford_capx_mapper_form_get_field_mapping_form($form, $form_state, $mapper);

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save mapping',
  );

  if (isset($mapper)) {
    $form['actions']['delete'] = array(
      '#markup' => l(t('Delete'), 'admin/config/capx/mapper/delete/' . $mapper->getMachineName(), array("attributes" => array("class" => array("button")))),
   );
  }
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
function stanford_capx_mapper_form_get_machine_name_form($form, &$form_state, $mapper = NULL) {

  $form['naming'] = array(
    '#title' => t('Mapping title'),
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#description' => t('Provide a human-readable name and machine name for this set of mapping configuration.'),
  );

  $form['naming']['title'] = array(
    '#type' => 'textfield',
    '#description' => isset($mapper->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t('Please enter a unique name for this mapper'),
    '#default_value' => isset($mapper->title) ? $mapper->title : '',
    '#disabled' => isset($mapper->machine_name),
  );

  $form['naming']['machine-name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#default_value' => isset($mapper->machine_name) ? $mapper->machine_name : '',
    '#size' => 64,
    '#maxlength' => 64,
    '#description' => isset($mapper->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t('A unique name for the mapping. It must only contain lowercase letters, numbers and hyphens.'),
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
function stanford_capx_mapper_form_get_entity_mapping_form($form, &$form_state, $mapper = NULL) {

  $entity_types = capx_entity_get_info();
  $entity_options = array();
  $bundle_options = array();

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
    '#description' => t('Select which entity type and bundle you would like to map CAP data into. Please select the entity type first and the bundle type will appear. The entity and bundle has to exist before creating a mapping to it.'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );

  $form['entity-mapping']['entity-type'] = array(
    '#type' => 'select',
    '#title' => t("Select entity type"),
    '#description' => isset($mapper->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t(''),
    '#options' => $entity_options,
    '#default_value' => isset($mapper->entity_type) ? $mapper->entity_type : 'node',
    '#disabled' => isset($mapper->machine_name),
  );

  // Build out the bundles options.
  foreach ($bundle_options as $entity_name => $bundle_opts) {

    $form['entity-mapping']['bundle-'.$entity_name] = array(
      '#type' => 'select',
      '#title' => t('Select bundle'),
      '#description' => isset($mapper->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t(''),
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
function stanford_capx_mapper_form_get_field_mapping_form($form, &$form_state, $mapper = NULL) {

  $entity_types = capx_entity_get_info();
  $field_collections = array();
  global $CAP_RESTRICTED_FIELD_PROPERTIES;

  $form['field-mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Field Mapping'),
    '#description' => t(''),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#tree' => TRUE,
  );

  $form['field-mapping']['help']['#markup'] = "<p>" . t("Using the fields below you are able to pull in data from the CAP API into your site. You do not have to input a setting for every field. Only use the fields that you want. The fields that you are importing data from the CAP API into will be locked and can only be changed by chaning the data on the CAP website.") . "</p>";
  $form['field-mapping']['help']['#markup'] .= "<p>" . t('Use the data browser to find the information you would like to map into the various different properties and fields below. You will need to use JSONPath syntax. For more information on JSONPath please read the documentation. ') . l('JSONPath Documentation →', 'http://goessner.net/articles/JsonPath/', array('attributes' => array('target' => '_blank'))) . "</p>";


  // ******************************************************
  // **** WARNING ***** BIG HEAVY FORM ***** WARNING ******
  // ******************************************************

  // PROPERTIES
  // ---------------------------------------------------------------------
  foreach ($entity_types as $entity_machine_name => $entity_info_values) {

    // If this is an edit mapper form (mapper exists) do not render the other
    // entities or bundles.

    if (isset($mapper->settings['entity_type']) && $mapper->settings['entity_type'] !== $entity_machine_name) {
      continue;
    }

    // Get all the properties of the entity type.
    $entity_info = entity_get_property_info($entity_machine_name);

    $property_container = $entity_machine_name . "-properties";

    // Generate property forms for each entity + bundle.
    $form['field-mapping'][$property_container] = array(
      '#type' => 'fieldset',
      '#title' => t('@name Properties', array("@name" => $entity_machine_name)),
      '#description' => t(''),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          'select[name="entity-type"]' => array('value' => $entity_machine_name),
        ),
      ),
    );

    $properties = isset($entity_info['properties']) ? $entity_info['properties'] : array();
    if (!empty($properties)) {
      $properties = stanford_capx_filter_properties($entity_machine_name, $properties);
    }

    foreach ($properties as $property_machine_name => $prop_info) {

      $form['field-mapping'][$property_container][$property_machine_name] = array(
        '#type' => 'container',
        '#title' => check_plain($prop_info['label']),
        '#field_info' => $prop_info,
        '#theme_wrappers' => array('stanford_capx_columns_to_the_right'),
        '#tree' => FALSE,
      );

      $form['field-mapping'][$property_container][$property_machine_name][$property_machine_name] = array(
        '#type' => 'textfield',
        '#title' => t('value'),
        '#description' => check_plain($prop_info['description']),
        '#default_value' => isset($mapper->properties[$property_machine_name]) ? $mapper->properties[$property_machine_name] : '',
        '#field_info' => $prop_info,
        '#parents' => array('field-mapping', $property_container, $property_machine_name),
      );

      if (isset($prop_info['required'])) {
        $form['field-mapping'][$property_container][$property_machine_name]['#attributes']['class'][] = "required";
      }

    }

  }

  // FIELDS
  // ---------------------------------------------------------------------
  foreach ($entity_types as $entity_machine_name => $entity_info_values) {

    // If this is an edit mapper form (mapper exists) do not render the other
    // entities or bundles.
    if (isset($mapper->settings['entity_type']) && $mapper->settings['entity_type'] !== $entity_machine_name) {
      continue;
    }

    // Get all the properties of the entity type.
    $entity_info = entity_get_property_info($entity_machine_name);

    // Skip those entities with no bundles.
    if (!isset($entity_info['bundles'])) {
      continue;
    }

    foreach ($entity_info['bundles'] as $bundle_machine_name => $bundle_info) {

      // Skip bundles that dont match a loaded mapper.
      if (isset($mapper->settings['bundle_type']) && $bundle_machine_name !== $mapper->settings['bundle_type']) {
        continue;
      }

      $fields = $entity_info['bundles'][$bundle_machine_name]['properties'];
      $field_container = $entity_machine_name . "-" . $bundle_machine_name . "-fields";

      // Generate property forms for each entity + bundle.
      $form['field-mapping'][$field_container] = array(
        '#type' => 'fieldset',
        '#title' => t("@var Fields", array("@var" => $bundle_machine_name)),
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
          '#title' => t("@var", array("@var" => $field_instance_info['label'])),
          '#description' => t("@var", array("@var" => $field_instance_info['description'])),
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

          if ($field_instance_info['required']) {
            $form['field-mapping'][$field_container][$field_machine_name]['#attributes']['class'][] = "required";
          }

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
              '#title' => t("@var", array("@var" => $column_key)),
              '#description' => !empty($column_info['description']) ? t("@var", array("@var" => $column_info['description'])) : "",
              '#default_value' => isset($mapper->fields[$field_machine_name][$column_key]) ? $mapper->fields[$field_machine_name][$column_key] : "",
            );

            if ($field_instance_info['required']) {
              $form['field-mapping'][$field_container][$field_machine_name]['#attributes']['class'][] = "required";
            }
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
          '#title' => t('@name Field Collection', array("@name" => $field_machine_name)),
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
            '#title' => t("@var", array("@var" => $instance_info['label'])),
            '#description' => t("@var", array("@var" => $instance_info['description'])),
            '#field_info' => $fc_field_info,
            '#field_instance_info' => $instance_info,
            '#theme_wrappers' => array('stanford_capx_columns_to_the_right'),
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
                '#title' => t("@var", array("@var" => $column_key)),
                '#description' => t(@$column_info['description']),
                '#default_value' => @$mapper->collections[$field_machine_name][$fc_field_name][$column_key],
              );

              if ($instance_info['required']) {
                $form['field-mapping'][$collection_container][$field_machine_name][$fc_field_name][$column_key]["#attributes"]["class"] = "required";
              }

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

  // Blocks / Beans.
  $blacklist['bean'][] = "bid";
  $blacklist['bean'][] = "vid";
  $blacklist['bean'][] = "delta";
  $blacklist['bean'][] = "view_mode";
  $blacklist['bean'][] = "type";
  $blacklist['bean'][] = "url";
  $blacklist['bean'][] = "uid";
  $blacklist['bean'][] = "created";
  $blacklist['bean'][] = "changed";
  $blacklist['bean'][] = "data";
  $blacklist['bean'][] = "body"; // body is not a property!

  // Field collection item.
  $blacklist['field_collection_item'][] = "item_id";
  $blacklist['field_collection_item'][] = "revision_id";
  $blacklist['field_collection_item'][] = "field_name";
  $blacklist['field_collection_item'][] = "archived";
  $blacklist['field_collection_item'][] = "url";
  $blacklist['field_collection_item'][] = "host_entity";

  // Nodes.
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
  $blacklist['node'][] = "feed_nid";
  $blacklist['node'][] = "uuid";
  $blacklist['node'][] = "vuuid";
  $blacklist['node'][] = "log";
  $blacklist['node'][] = "body"; // body is not a property!

  // Terms.
  $blacklist['taxonomy_term'][] = "tid";
  $blacklist['taxonomy_term'][] = "weight";
  $blacklist['taxonomy_term'][] = "node_count";
  $blacklist['taxonomy_term'][] = "url";
  $blacklist['taxonomy_term'][] = "vocabulary";
  $blacklist['taxonomy_term'][] = "parent";
  $blacklist['taxonomy_term'][] = "parents_all";
  $blacklist['taxonomy_term'][] = "created";
  $blacklist['taxonomy_term'][] = "changed";
  $blacklist['taxonomy_term'][] = "body"; // body is not a property!

  // User.
  $blacklist['user'][] = "uid";
  $blacklist['user'][] = "url";
  $blacklist['user'][] = "edit_url";
  $blacklist['user'][] = "last_access";
  $blacklist['user'][] = "last_login";
  $blacklist['user'][] = "roles";
  $blacklist['user'][] = "status";
  $blacklist['user'][] = "theme";
  $blacklist['user'][] = "created";
  $blacklist['user'][] = "changed";
  $blacklist['user'][] = "body"; // body is not a property!

  // Anything we don't know about.
  $blacklist['unknown'][] = "id";
  $blacklist['unknown'][] = "vid";
  $blacklist['unknown'][] = "url";
  $blacklist['unknown'][] = "edit_url";
  $blacklist['unknown'][] = "uid";
  $blacklist['unknown'][] = "changed";
  $blacklist['unknown'][] = "created";
  $blacklist['unknown'][] = "body"; // body is not a property!


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

  // Entities have fields and properties.
  stanford_capx_mapper_form_validate_element($form_state['values']['field-mapping'][$entity . "-properties"], array('field-mapping'));

  if (isset($form_state['values']['field-mapping'][$entity . "-" . $bundle . "-fields"])) {
    stanford_capx_mapper_form_validate_element($form_state['values']['field-mapping'][$entity . "-" . $bundle . "-fields"], array('field-mapping'));
  }

  // Only some have field collections.
  if (isset($form_state['values']['field-mapping'][$entity . "-" . $bundle .  "-collections"])) {
    stanford_capx_mapper_form_validate_element($form_state['values']['field-mapping'][$entity . "-" . $bundle . "-collections"], array('field-mapping'));
  }

  // Set the error messages after everything has been looped through.
  stanford_capx_mapper_form_validate_element_set_messages();

  // Ensure all required fields were posted. Because of the way the form works
  // we cannot place a required on to each of the form element fields as there
  // are a number of fields that are hidden and we do not want them. They still
  // need to be filled out.
  stanford_capx_mapper_form_validate_required_fields($form, $form_state, $entity, $bundle);

}

/**
 * Ensure all required fields were posted.
 *
 * Because of the way the form works we cannot place a required on to each of
 * the form element fields as there are a number of fields that are hidden and
 * we do not want them. They still need to be filled out.
 *
 * @param array $form
 *   Form array
 * @param array $form_state
 *   Form state array
 * @param string $entity
 *   The machine name of the entity type to be validated.
 * @param string $bundle
 *   The machine name of the bundle type to be validated.
 */
function stanford_capx_mapper_form_validate_required_fields(&$form, &$form_state, $entity, $bundle) {

  // Get all the properties of the entity type.
  $entity_info = entity_get_property_info($entity);
  $properties = $entity_info['properties'];

  $fields = isset($entity_info['bundles']) ? $entity_info['bundles'][$bundle]['properties'] : array();

  // Properties validation.
  foreach ($form_state['values']['field-mapping'][$entity . "-properties"] as $k => $v) {
    if (empty($v) && !empty($properties[$k]['required'])) {
      $element = 'field-mapping][' . $entity . "-properties][" . $k;
      form_set_error($element, t('@name property is required.', array("@name" => $properties[$k]['label'])));
    }
  }

  // Field validation.
  foreach ($fields as $field_name => $value) {

    if (isset($value['required']) && $value['required']) {

      if (!isset($form_state['values']['field-mapping'][$entity . "-" . $bundle . "-fields"][$field_name])) {
        continue;
      }

      $columns = $form_state['values']['field-mapping'][$entity . "-" . $bundle . "-fields"][$field_name];

      // Loop through the columns and check for values.
      foreach ($columns as $column_name => $value) {
        if (empty($value) && $column_name != 'summary') {
          $element = "field-mapping][" . $entity . "-" . $bundle . "-" . "fields][" . $field_name . "][" . $column_name;
          form_set_error($element, t("@name field is required", array("@name" => $field_name)));
          break;
        }
      }
    }
  }

}

/**
 * Validate the user input.
 *
 * Validate the user input from the mapper form by checking to see if the
 * selector finds something in the API schema.
 *
 * @param  [type] $element [description]
 * @param  array  $parents [description]
 * @return [type]          [description]
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
        // Empty array is returned for an invalid selector.
        if (empty($data_type) && !_stanford_capx_mapper_is_valid_path($child_element, $json_parser)) {
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
      t('The following selectors did not match an element in the profile schema but may be valid. If you know the selector is valid please disregard this message. Try using the !databrowser to find a valid selector.!warnings',
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

  // If there are field collections check to see if they are any values in them.
  // if (!empty($collections)) {
  //   foreach ($collections as $collection => $fields) {
  //     foreach ($fields as $field_name => $values) {
  //       $value = array_pop($values);
  //       if (empty($value)) {
  //         unset($collections[$collection][$field_name]);
  //       }
  //     }
  //     if (empty($collections[$collection])) {
  //       unset($collections[$collection]);
  //     }
  //   }
  // }

  // We have values!
  if (!empty($collections)) {
    $settings['collections'] = $collections;
  }

  $mapper->settings = $settings;

  // New vs Edit.
  if (!empty($mapper->is_new)) {
    drupal_set_message(t('Congratulations, Your mapping has been created.'), 'status', FALSE);
  }
  else {
    CAPx::invalidateEtags("mapper", $mapper);
    drupal_set_message(t("Your mapping has been updated."), 'status', FALSE);
  }

  // Do the save.
  capx_mapper_save($mapper);

  // Lets check the error messages that may be related to this mapper.
  $messages = variable_get('stanford_capx_admin_messages', array());
  foreach ($messages as $key => &$message) {
    $message_meta = explode(':', $key);
    // When the field is missing from some bundle in
    // \CAPx\Drupal\Mapper\EntityMapper::mapFields we creating key from 4
    // components:
    // - mapper machine name
    // - entity type
    // - entity bundle
    // - entity field name
    // In this exact order.
    if (count($message_meta) === 4) {
      if (
        $message_meta[0] == $values['machine-name']
        && $message_meta[1] == $entity
        && $message_meta[2] == $bundle
      ) {
        unset($messages[$key]);
      }
    }
    elseif (!empty($message['mappers']) && array_key_exists($values['machine-name'], $message['mappers'])) {
      unset($message['mappers'][$values['machine-name']]);

      if (empty($message['mappers'])) {
        // This was the last mapper that were using deleted field.
        unset($messages[$key]);
      }
      else {
        $message['message_vars']['!mappers'] = implode(', ', $message['mappers']);
      }
    }
  }
  variable_set('stanford_capx_admin_messages', $messages);

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
  $description = t('This action cannot be undone.');
  $name = 'confirm_mapper_delete';
  $path = "admin/config/capx/mapper";

  if (!$mapper) {
    throw new Exception("Could not load a mapping with provided machine name. Please check url.");
  }

  $question = "Are you sure you want to delete " . $mapper->title . "?";
  $importers = CAPxImporter::loadImportersByMapper($mapper);
  $items = array();

  foreach ($importers as $importer) {
    $items[] = l($importer->title, 'admin/config/capx/importer/edit/' . $importer->identifier());
  }

  if (count($items)) {
    $list = array(
      'title' => 'This mapping is being used by these importers:',
      'items' => $items,
    );
    $form['importers'] = array(
      '#markup' => theme('item_list', $list),
    );
    $description = t("You cannot delete this mapper until it is no longer being used by an importer. Please change the importer mapping option or delete the importer before proceeding.");
  }

  $form['machine-name'] = array(
    '#type' => 'hidden',
    '#value' => $mapper->machine_name,
  );


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
  drupal_set_message(t('Mapping was deleted.'), 'status');
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
function stanford_capx_importer_form($form, &$form_state, $importer = NULL) {

  // Do a check to see if the organization codes are available. If not, prompt
  // the user to update them.
  $has_orgs = \CAPx\Drupal\Organizations\Orgs::checkOrgs();
  if (!$has_orgs) {
    $link = l('Sync with the API now.', 'admin/config/capx/organizations/sync', array('query' => array('destination' => current_path())));
    drupal_set_message(t('The organization codes are not available. !link', array("!link" => $link)), 'warning');
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
    '#description' => isset($importer->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t('Please enter a unique name for this Importer'),
    '#default_value' => isset($importer->title) ? $importer->title : '',
    '#disabled' => isset($importer->machine_name),
    '#required' => TRUE,
  );

  $form['naming']['machine-name'] = array(
    '#type' => 'machine_name',
    '#title' => t('Machine name'),
    '#default_value' => isset($importer->machine_name) ? $importer->machine_name : '',
    '#size' => 64,
    '#maxlength' => 64,
    '#description' => isset($importer->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t('A unique name for the importer. It must only contain lowercase letters, numbers and hyphens.'),
    '#machine_name' => array(
      'exists' => 'stanford_capx_importer_machine_name_exits',
      'source' => array('naming','title'),
    ),
    '#disabled' => isset($importer->machine_name),
  );

  // Mapper options.
  $mapping_options = array();
  $mappers = CAPxMapper::loadAllMappers();

  if (!count($mappers)) {
    drupal_set_message(t("No mappings found. Please !maplink before proceeding with this importer.", array("!maplink" => l(t("create a mapping"), "admin/config/capx/mapper/new"))), "error");
  }

  foreach ($mappers as $mapper) {
    $mapping_options[$mapper->machine_name] = $mapper->title;
  }

  $form['naming']['mapper'] = array(
    '#type' => 'select',
    '#title' => t('Mapping'),
    '#description' => isset($importer->mapper) ? t("Changing this field can have adverse consquences to your data. Take great care before changing this field.") : t('Choose how you would like to import this profile(s) data.'),
    '#options' => $mapping_options,
    '#default_value' => isset($importer->mapper) ? $importer->mapper : '',
    '#required' => TRUE,
    '#empty_option' => t('--Please select--'),
    '#ajax' => array(
      'wrapper' => 'orphan-action-wrapper',
      'callback' => '_stanford_capx_importer_form_mapper_ajax_callback'
    ),
  );

  // Orphan action section.
  $action_options = array(
    'nothing' => t('Nothing'),
  );
  $default_action = 'nothing';
  if (empty($form_state['values']['mapper'])) {
    $mapper = isset($importer) ? $importer->mapper : NULL;
  }
  else {
    $mapper = $form_state['values']['mapper'];
  }
  if (isset($mapper)) {
    $mapper = capx_cfe_load_by_machine_name($mapper, 'mapper');
    $entity_type = $mapper->entity_type;
    switch ($entity_type) {
      case 'node':
        $default_action = 'unpublish';
        $action_options[$default_action] = t('Unpublish node');
        break;

      case 'user':
        $default_action = 'block';
        $action_options[$default_action] = t('Block user');
        break;
    }
  }
  // Adding this option here to keep meaningful order for the end user.
  $action_options['delete'] = t('Delete');

  // Cron Settings
  // -–-–-–-–-–-–-–-–-–-–-

  $form['cron'] = array(
    '#title' => t('Automatic synchronization options (CRON)'),
    '#description' => t("The CAPx module is set up to provide automatic synchronization of your profiles. Below are a number of controls to adjust how often they are updated. These options require cron to be configured. For more information about setting up and configuring cron please see the !docs", array("!docs" => l(t("drupal documenation."), "https://www.drupal.org/cron"))),
    '#type' => 'fieldset',
    '#collapsed' => !empty($importer),
    '#collapsible' => TRUE,
  );

  $form['cron']['cron_option'] = array(
    '#title' => t('Interval'),
    '#description' => t('Select an interval on how often this importer should syncronize with the CAP server. This setting is dependant on !cron.', array('!cron' => l(t('cron configuration'), 'admin/config/system/cron'))),
    '#type' => 'select',
    '#options' => CAPxImporter::getCronOptions(),
    '#default_value' => isset($importer->cron_option) ? $importer->cron_option : 'daily',
  );

  $form['cron']['cron_option_hour'] = array(
    "#type" => "textfield",
    "#title" => t("At hour of the day"),
    "#description" => t("Please enter the hour of the day you wish this to run at. eg: 3:00am or 18:00"),
    "#default_value" => isset($importer->cron_option_hour) ? $importer->cron_option_hour : "3:00",
    "#states" => array(
      "visible" => array(
        ":input[name='cron_option']" => array(
          array("value" => "daily"),
          array("value" => "weekly"),
          array("value" => "monthly"),
          array("value" => "yearly"),
        ),
      ),
    ),
  );

  $form['cron']['cron_option_day_week'] = array(
    "#type" => "select",
    "#title" => t("At day of the week"),
    "#description" => t("Select the day of the week you wish to sync on."),
    "#options" => array(
      "monday" => t("Monday"),
      "tuesday" => t("Tuesday"),
      "wednesday" => t("Wednesday"),
      "thursday" => t("Thursday"),
      "friday" => t("Friday"),
      "saturday" => t("Saturday"),
      "sunday" => t("Sunday")),
    "#default_value" => isset($importer->cron_option_day_week) ? $importer->cron_option_day_week : "Sunday",
    "#states" => array(
      "visible" => array(
        ":input[name='cron_option']" => array(
          array("value" => "weekly"),
        ),
      ),
    ),
  );

  $form['cron']['cron_option_day_number'] = array(
    "#type" => "textfield",
    "#title" => t("At day of the month"),
    "#description" => t("Select the day of the month you wish to sync on. eg: 23"),
    "#default_value" => isset($importer->cron_option_day_number) ? $importer->cron_option_day_number : "1",
    "#states" => array(
      "visible" => array(
        ":input[name='cron_option']" => array(
          array("value" => "monthly"),
          array("value" => "yearly"),
        ),
      ),
    ),
  );

  $form['cron']['cron_option_month'] = array(
    "#type" => "select",
    "#title" => t("At month of the year"),
    "#description" => t("Select the month of the year you wish to sync on."),
    "#options" => array(t("January"), t("February"), t("March"), t("April"), t("May"), t("June"), t("July"), t("August"), t("September"), t("October"), t("November"), t("December")),
    "#default_value" => isset($importer->cron_option_month) ? $importer->cron_option_month : "January",
    "#states" => array(
      "visible" => array(
        ":input[name='cron_option']" => array(
          array("value" => "yearly"),
        ),
      ),
    ),
  );

  $form['cron']['orphan_action'] = array(
    '#title' => t('Action to perform on orphaned profiles'),
    '#description' => t('Orphan profiles are profiles that are removed from the CAP API or from the importer configuration. Unpublished option only works on node entities and all other types will be left unchanged.'),
    '#type' => "select",
    '#options' => $action_options,
    '#default_value' => $default_action,
    '#prefix' => '<div id="orphan-action-wrapper">',
    '#suffix' => '</div>',
  );

  // Profile group
  // -–-–-–-–-–-–-–-–-–-–-

  $form['groups'] = array(
    '#title' => t('Add groups and individuals'),
    '#description' => t("You can add any combination of groups or individuals. The following options are additive. They will combine together and import all individuals within those options. If a person or profile exists in more than one of the options their profile will not be duplicated. Only one profile per person per importer can exist. You do not have to worry about duplicates."),
    '#type' => 'fieldset',
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
  );

  $link = l(t('Stanford University Org Code Hierarchy'), 'http://www.stanford.edu/dept/pres-provost/budget/org/orgchart/');

  $form['groups']['organization'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization(s)'),
    '#description' => t('Please select the name(s) of the organization(s) you wish to import. Enter multiple organizations by separating them with a comma ",".') . "<br />" . t('Learn more about !orgchart.', array('!orgchart' => $link)),
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
    '#description' => t('Enter the name(s) of the workgroup(s) you wish to import. Enter multiple organizations by separating them with a comma ",".') . "<br />" . t('You can learn more about workgroups at Stanford, and get propernames for import, at the !managerlink.', array('!managerlink' => $link)),
    '#default_value' => isset($importer->workgroup) ? $importer->workgroup : '',
  );

  // $form['individuals'] = array(
  //   '#title' => t('Add individuals'),
  //   '#type' => 'fieldset',
  //   '#collapsed' => FALSE,
  //   '#collapsible' => FALSE,
  //   '#description' => t(''),
  // );

  $form['groups']['sunet_id'] = array(
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

  if (isset($importer)) {
     $form['actions']['delete'] = array(
      '#markup' => l(t('Delete'), 'admin/config/capx/importer/delete/' . $importer->getMachineName(), array("attributes" => array("class" => array("button")))),
   );
  }


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

  return $form;
}

/**
 * AJAX callback.
 */
function _stanford_capx_importer_form_mapper_ajax_callback($form, $form_state) {
  return $form['cron']['orphan_action'];
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
  $settings['cron_option_day_number'] = $values['cron_option_day_number'];
  $settings['cron_option_day_week'] = $values['cron_option_day_week'];
  $settings['cron_option_month'] = $values['cron_option_month'];
  $settings['cron_option_hour'] = $values['cron_option_hour'];
  $settings['orphan_action'] = $values['orphan_action'];

  $importer->settings = $settings;

  // New vs Edit.
  if (!empty($importer->is_new)) {
    drupal_set_message(t('Congratulations, Your importer has been created.'), 'status', FALSE);
  }
  else {
    CAPx::invalidateEtags("importer", $importer);
    drupal_set_message(t("Your importer has been updated."), 'status', FALSE);
  }

  // Do the save.
  capx_importer_save($importer);
  $form_state['redirect'] = 'admin/config/capx/importer';

  // Lets check the error messages that may be related to this importer.
  $messages = variable_get('stanford_capx_admin_messages', array());
  foreach ($messages as $key => &$message) {
    if (!empty($message['importers']) && array_key_exists($values['machine-name'], $message['importers'])) {
      unset($message['importers'][$values['machine-name']]);

      if (empty($message['importers'])) {
        // This was the last importer that were using deleted mapper.
        unset($messages[$key]);
      }
      else {
        $message['message_vars']['!importers'] = implode(', ', $message['importers']);
      }
    }
  }
  variable_set('stanford_capx_admin_messages', $messages);
}

/**
 * Submit callback function that runs the batch api import for this importer
 * @return [type] [description]
 */
function stanford_capx_importer_form_submit_run_import($form, $form_state) {
  $values = $form_state['values'];
  $importer = CAPxImporter::loadEntityImporter($values['machine-name']);

  if (!$importer) {
    $vars = array(
      '%name' => $values['machine-name'],
      '!log' => l(t('log messages'), 'admin/reports/dblog'),
    );
    drupal_set_message(t('There was an issue loading the importer with %name machine name. Check !log.', $vars), 'error');
    return;
  }

  if (!$importer->valid()) {
    drupal_set_message(t('There was an issue running the importer.'), 'error');
  }
  else {
    $importer->createBatch();
    drupal_set_message(t('Items successfully imported'), 'status');
  }
}

/**
 * Form callback for confirmation step about deleting an importer.
 *
 * Form callback confirms the action of deleting and allows the user to choose
 * what they want to do with the profiles that are associated with it.
 *
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_admin_config_importer_delete($form, &$form_state, $machine_name) {

  // Get the importer.
  $importer = CAPxImporter::loadEntityImporter($machine_name);


  // No importer, no need to continue.
  if (!$importer) {
    $vars = array(
      '%name' => $machine_name,
      '!log' => l(t('log messages'), 'admin/reports/dblog'),
    );
    drupal_set_message(t('There was an issue loading the importer with %name machine name. Check !log.', $vars), 'error');
    drupal_goto('admin/config/capx/importer');
  }

  // Add the machine name so we can do stuff with the importer later.
  $form['machine-name'] = array(
    '#type' => 'value',
    '#value' => $machine_name,
  );

  // Operations.
  $form['profile_action'] = array(
    '#type' => 'select',
    '#title' => t('What would you like to do with the items that are associated with this importer?'),
//    '#description' => t('Perform an action on all of the items that have been imported by this importer.'),
    '#options' => array(
      'nothing' => t('Do nothing and leave the items as they are'),
      'delete' => t('Delete all of the items'),
    ),
  );

  // Provide the unpublish action for the node & user types only.
  if ($importer->getEntityType() == "node" || $importer->getEntityType() == "user") {
    $form['profile_action']['#options']['unpublish'] = t('Unpublish the items');
  }

  // Confirm form stuff.
  $question = "Are you sure you want to delete " . $importer->getImporter()->title . "?";
  $path = "admin/config/capx/importer";
  $description = t('This action cannot be undone.');
  $name = 'confirm_importer_delete';

  $form = confirm_form($form, $question, $path, $description, t('Yes, please delete the importer'), t('Cancel'), $name);
  return $form;
}

/**
 * Delete the importer callback.
 *
 * If the importer was confirmed to be deleted we need to take action on the
 * profiles.
 *
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_admin_config_importer_delete_submit($form, $form_state) {

  $machine_name = $form_state['values']['machine-name'];
  $entity_importer = CAPxImporter::loadEntityImporter($machine_name);
  $type = $entity_importer->getEntityType();

  $importer = $entity_importer->getImporter();
  $profile_op = check_plain($form_state['values']['profile_action']);

  switch ($profile_op) {

    case 'unpublish':
      $profiles = CAPx::getProfilesByImporter($machine_name);
      $count = count($profiles);
      foreach ($profiles as $profile) {
        // Set the status of the node/user to 0. A check to ensure that this
        // option is only available for nodes and users is in the form itself.
        $profile->status = 0;

        // Wrap it.
        $wrapper = entity_metadata_wrapper($type, $profile);
        $wrapper->save();
      }
      drupal_set_message(t('@count @type were set to unpublished.', array("@count" => $count, "@type" => $type)), 'status');

      break;

    case "delete":
      $ids = CAPx::getProfileIds($type, array('importer' => $machine_name));
      $count = count($ids);
      entity_delete_multiple($type, $ids);
      drupal_set_message(t('@count @type were deleted.', array("@count" => $count, "@type" => $type)), 'status');
      break;

    case "nothing":
    default:
      drupal_set_message(t("All @type were left unchanged.", array("@type" => $type)), "status");
      break;
  }

  capx_cfe_delete($importer);

  drupal_set_message(t('Importer was deleted.'), 'status');
  drupal_goto('admin/config/capx/importer');
}

// /////////////////////////////////////////////////////////////////////////////
// FORM ALTERS
// /////////////////////////////////////////////////////////////////////////////

/**
 * Implements hook_form_alter().
 */
function stanford_capx_form_alter(&$form, &$form_state, $form_id) {
  // For supported entities add the vertical tab and disable fields that are
  // mapped.
  list($entity_type, $entity) = _stanford_capx_entity_form_get_entity($form, $form_state, $form_id);

  if (isset($entity->capx)) {
    drupal_set_message(t("This entity has been imported from the CAP API. Fields that are greyed out cannot be changed as that information is from the CAP website. If you would like to change that information please visit the CAP website and edit the information there."), "warning");
    stanford_capx_entity_form_add_vt($form, $form_state, $entity_type, $entity);
    stanford_capx_entity_form_disable_mapped_fields($form, $form_state, $entity_type, $entity);
  }
}

/**
 * Figure out entity type.
 *
 * @return array
 *   Entity type and entity of
 */
function _stanford_capx_entity_form_get_entity(&$form, &$form_state, $form_id) {
  $entity_type = NULL;
  $entity = NULL;

  switch ($form_id) {
    case "bean_form":
      $entity_type = "bean";
      $entity = $form_state["bean"];
      break;

    case 'taxonomy_form_term':
      $entity_type = 'taxonomy_term';
      $entity =& $form_state['term'];
      break;

    case 'user_profile_form':
      $entity_type = 'user';
      $entity =& $form_state['user'];
      break;
  }

  if (isset($form['#node_edit_form']) && $form['#node_edit_form']) {
    $entity_type = 'node';
    $entity =& $form_state['node'];
  }

  return array($entity_type, $entity);
}

/**
 * Adds CAPx info to supported entity forms.
 */
function stanford_capx_entity_form_add_vt(&$form, &$form_state, $entity_type, $entity) {

  $capx_info = $entity->capx;
  $importer = capx_cfe_load_by_machine_name($capx_info['importer'], 'importer');
  if (!$importer) {
    return;
  }

  // Create a fieldset that will be included in the vertical tab.
  $form['capx_vertical_tab'] = array(
    '#type' => 'fieldset',
    '#title' => t('CAPx Information'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#weight' => -99,
    '#description' => t(''),
    '#summary' => t('summary'),
    // The #group value must match the name of the vertical tabs element.
    // In most cases, this is 'additional_settings'.
    '#group' => 'additional_settings',
    '#attached' => array(
      'css' => array(
        drupal_get_path('module', 'stanford_capx') . '/css/stanford_capx.entity_form.css',
      ),
    ),
  );

  $form['capx_vertical_tab']['last_update'] = array(
    '#prefix' => '<p class="last-update"><span class="label">' . t('Last update:') . '</span><span class="info">',
    '#markup' => format_date($capx_info['last_sync'], 'full'),
    '#suffix' => '</span></p>',
  );

  $importer_label = entity_label('capx_cfe', $importer);
  $importer_path = 'admin/config/capx/importer/edit/' . $capx_info['importer'];
  $mapper = capx_cfe_load_by_machine_name($importer->mapper, 'mapper');
  $mapper_label = entity_label('capx_cfe', $mapper);
  $mapper_path = 'admin/config/capx/mapper/edit/' . $importer->mapper;
  $vars = array(
    '!importer' => l(check_plain($importer_label), $importer_path),
    '!mapper' => l(check_plain($mapper_label), $mapper_path),
  );
  $form['capx_vertical_tab']['importer'] = array(
    '#prefix' => '<p class="importer"><span class="label">' . t('Imported by:') . '</span><span class="info">',
    '#markup' => t('!importer using !mapper mapping.', $vars),
    '#suffix' => '</span></p>',
  );

  // Sync now button.
  if (user_access('administer capx')) {
    $path = 'admin/config/capx/profile-update/' . $capx_info['profile_id'] . '/' . $capx_info['importer'];
    $options = array(
      'attributes' => array(
        'class' => array('button'),
        ),
      'query' => array(
        'destination' => $_GET['q'],
        ),
      );
    $form['capx_vertical_tab']['update']['#markup'] = "<p>" . l(t('Update this profile from CAP'), $path, $options) . "<br />";
    $form['capx_vertical_tab']['update']['#markup'] .= t("Clicking on the above button will update this profile with date from the CAP API wether or not sync is enabled or disabled") . "</p>";
  }

  $status = empty($capx_info['sync']) ? t('Not receiving updates from CAP.') : t('Receiving updates from CAP.');
  $form['capx_vertical_tab']['status'] = array(
    '#prefix' => '<p class="status"><span class="label">' . t('Status:') . '</span><span class="info">',
    '#markup' => $status,
    '#suffix' => '</span></p>',
  );


  if (user_access('administer capx')) {
    $sync = empty($entity->capx['sync']) ? TRUE : FALSE;
    $button_text = $sync ? t('Enable automatic updates from CAP for this profile') : t('Disable automatic updates from CAP for this profile');
    $path = 'admin/config/capx/profile-sync/' . $entity->capx['id'] . '/';
    $path .= $sync ? 'enable' : 'disable';
    $options = array(
      'attributes' => array(
        'class' => array('button'),
      ),
      'query' => array(
        'destination' => check_plain($_GET['q']),
      ),
    );
    $form['capx_vertical_tab']['disable_sync']['#markup'] = "<p>" . l($button_text, $path, $options) . "<br />";
    $form['capx_vertical_tab']['disable_sync']['#markup'] .= t("Disabling automatic updates from the CAP API will release the locked fields and allow you to edit them. Please be aware that enabling or re-syncing with the API could overwrite these changes.") . "</p>";
  }
}

/**
 * Disable all fields that have mapped data.
 */
function stanford_capx_entity_form_disable_mapped_fields(&$form, &$form_state, $entity_type, $entity) {

  // Only proceed if the sync is enabled.
  if (!$entity->capx['sync']) {
    return;
  }

  $importer = CAPxImporter::loadImporter($entity->capx['importer']);
  $mapper = CAPxMapper::loadMapper($importer->mapper);

  $fields = $mapper->fields;
  $properties = $mapper->properties;
  $collections = $mapper->collections;

  $description = t('This field is mapped with CAP data and cannot be changed through the UI. Please visit the CAP website to update this field.');

  // Disable any properties.
  foreach ($properties as $property_name => $value) {
    if (!empty($value)) {
      if ($entity_type == 'user') {
        $form['account'][$property_name]['#disabled'] = TRUE;
        $form['account'][$property_name]['#description'] = $description;
      }
      else {
        $form[$property_name]['#disabled'] = TRUE;
        $form[$property_name]['#description'] = $description;
      }
    }
  }

  // Disable any fields with data.
  foreach ($fields as $field_name => $columns) {
    foreach ($columns as $value) {
      if (!empty($value)) {
        $lng = field_language($entity_type, $entity, $field_name);

        $form[$field_name]['#disabled'] = TRUE;
        $form[$field_name][$lng]['#description'] = $description;
        foreach (element_children($form[$field_name][$lng]) as $key) {
          $form[$field_name][$lng][$key]['#description'] = $description;
          foreach (element_children($form[$field_name][$lng][$key]) as $column) {
            $form[$field_name][$lng][$key][$column]['#description'] = $description;
          }
        }
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
 * [stanford_capx_views_exposed_form_alter description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_form_views_exposed_form_alter(&$form, &$form_state) {

  // Only run on profiles listing view.
  if ($form['#id'] !== "views-exposed-form-capx-admin-profiles-profiles") {
    return;
  }

  // Change the entity type and bundle type into drop downs.
  $entity_info = capx_entity_get_info();

  // Define no value.
  $entity_options = array('' => t("- Any -"));
  $bundle_options = array('' => t("- Any -"));

  // Loop through information and populate options.
  foreach ($entity_info as $entity_name => $info) {
    $entity_options[$entity_name] = t("@title", array("@title" => $info['label']));
    foreach ($info['bundles'] as $bundle_name => $bundle_info) {
      $bundle_options[$info['label']][$bundle_name] = t("@name", array("@name" => $bundle_info['label']));
    }
  }

  // Set up the entity type select.
  $form['entity_type']['#type'] = "select";
  $form['entity_type']['#options'] = $entity_options;
  unset($form['entity_type']['#size']);

  // Set up bundle type select.
  $form['bundle_type']['#type'] = "select";
  $form['bundle_type']['#options'] = $bundle_options;
  unset($form['bundle_type']['#size']);

}

// /////////////////////////////////////////////////////////////////////////////
// End Form Alters
// /////////////////////////////////////////////////////////////////////////////

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
 * The global configuration settings for CAPx form builder.
 */
function stanford_capx_config_settings_form($form, &$form_state) {

  $form['variables'] = array(
    '#type' => 'fieldset',
    '#title' => t("Synchronization settings"),
    '#description' => t("Settings for update and synchronization actions with the CAP API."),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['variables']['stanford_capx_batch_limit'] = array(
    '#title' => t('Batch, or cron, processing limit'),
    '#description' => t('The number of items to process on one sync operation.'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get("stanford_capx_batch_limit", 100),
  );

  $options = array();
  $formats = filter_formats();
  foreach ($formats as $format) {
    $options[$format->format] = $format->name;
  }
  $form['variables']['stanford_capx_default_field_format'] = array(
    '#title' => t('Text format'),
    '#description' => t('Select text format that would be applied to all fields that have text processing enabled.'),
    '#type' => "select",
    '#options' => $options,
    '#default_value' => variable_get('stanford_capx_default_field_format', 'filtered_html'),
  );

  $form = system_settings_form($form);
  $form['actions']['submit']['#value'] = t("Save settings");
  return $form;
}

/**
 * Validates wether or not a path is acceptible.
 *
 * @param string $path
 *   The path being looked up.
 * @param object $parser
 *   The JSONParser object
 *
 * @return bool
 *   True for valid and False for invalid.
 */
function _stanford_capx_mapper_is_valid_path($path, $parser) {
  $trimmed_path = preg_replace("/\[[0-9]\]/", "", $path);
  $valid_paths = _stanford_capx_valid_jsonpaths();
  $valid_patterns = _stanford_capx_valid_jsonpath_patterns();

  // If the path is in the whitelist then return TRUE.
  if (in_array($trimmed_path, $valid_paths)) {
    return TRUE;
  }

  // If the path validates after being trimmed return TRUE.
  $data_type = $parser->get($trimmed_path);
  if (!empty($data_type)) {
    return TRUE;
  }

  // If the path matches any of our patterns then return TRUE;
  foreach ($valid_patterns as $pattern) {
    if (preg_match($pattern, $trimmed_path)) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
 * Returns list of known valid JSONPaths not described in schema.
 */
function _stanford_capx_valid_jsonpaths() {
  return array(
    // '$.lastModified',
    // '$.documents.cv',
    // '$.documents.resume',
    // '$.affiliations',
    '$.profileID',
  );
}

/**
 * Returns an array of json path patterns for use in regex.
 */
function _stanford_capx_valid_jsonpath_patterns() {
  return array(
    '/\$\.documents\..*/',
    '/\$\.affiliations\..*/',
  );
}
