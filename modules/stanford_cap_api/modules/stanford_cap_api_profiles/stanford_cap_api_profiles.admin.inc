<?php
/**
 * @file
 * Administrative pages for Stanford CAP API module.
 */

define('CAP_PROFILES_ENTRY_PER_PAGE', 25);

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Adding info to details page.
 */
function stanford_cap_api_profiles_form_stanford_cap_api_details_form_alter(&$form, &$form_state) {
  $query = new EntityFieldQuery();
  $count = $query
    ->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'cap_stanford_profile')
    ->count()
    ->execute();
  $additional_vars = array(
    '!groups' => l(t('importing groups'), 'admin/config/cap/config/group'),
    '!people' => l(t('importing individual profiles'), 'admin/config/cap/config/import'),
  );
  $vars = array(
    'info' => $count,
    'message' => t('CAP profile(s) are being stored in your website.'),
    'additional' => t('You can add more by !groups or !people.', $additional_vars),
  );

  $form['profiles'] = array(
    '#markup' => theme('cap_info_item', $vars),
  );

  $sync_date_stamp = variable_get('stanford_cap_api_profiles_cron_last_run', 0);
  $additional = t('You can adjust the frequency of updates on the !settings.', array('!settings' => l(t('settings page'), 'admin/config/cap/config/settings')));
  if (empty($sync_date_stamp)) {
    $vars = array(
      'message' => t('Stanford profiles have not yet been synchronized.'),
      'additional' => $additional,
    );

    $form['sync_date'] = array(
      '#markup' => theme('cap_status_item', $vars),
    );
  }
  else {
    $vars = array(
      'info' => round((REQUEST_TIME - $sync_date_stamp) / 60 / 60),
      'message' => t('Hour(s) since last sync.'),
      'additional' => $additional,
    );

    $form['sync_date'] = array(
      '#markup' => theme('cap_info_item', $vars),
    );
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function stanford_cap_api_profiles_form_stanford_cap_api_settings_form_alter(&$form, &$form_state) {
  $form['#attached']['library'][] = array('stanford_cap_api_profiles', 'stanford_cap_api_profiles_fields');
  if (!variable_get('stanford_cap_api_profiles_schema_synchronized', FALSE)) {
    if (isset($form_state['values']) && isset($form_state['values']['stanford_cap_api_import_profile_fields']) && $form_state['values']['stanford_cap_api_import_profile_fields'] == CAP_FIELDS_IMPORT_LIST) {
      // Import of fields list was selected. Building fields list select form.
      $form['auth']['#access'] = FALSE;
      $form['description_wrapper']['description']['#markup'] = t('Please select the fields for CAP Profiles that will be synchronized with the CAP API.');
      $form['actions']['submit']['#value'] = t('Next');
      $form['actions']['submit']['#weight'] = 1;

      $form['actions']['back'] = array(
        '#type' => 'submit',
        '#value' => t('Back'),
        '#executes_submit_callback' => FALSE,
        '#weight' => 1,
      );

      $form['fields'] = array(
        '#theme' => 'stanford_cap_api_profiles_field_tree',
        '#tree' => TRUE,
      );

      $schema = stanford_cap_api_profiles_get_profile_schema();
      // Basic check that we have an expected schema.
      if (is_array($schema) && $schema['type'] == 'object' && !empty($schema['properties'])) {
        _stanford_cap_api_profiles_field_checkboxes($schema['properties'], $form['fields']);
      }

    }
    else {
      $form['synchronization'] = array(
        '#type' => 'fieldset',
        '#title' => t('Synchronization preferences'),
      );

      $form['synchronization']['description_wrapper'] = array(
        '#type' => 'container',
      );
      $form['synchronization']['description_wrapper']['description'] = array(
        '#markup' => t('Please select the data import option that best reflects how you would like to use data from the CAP API.'),
      );

      $form['synchronization']['stanford_cap_api_import_profile_fields'] = array(
        '#type' => 'radios',
        '#default_value' => variable_get('stanford_cap_api_import_profile_fields', CAP_FIELDS_IMPORT_ALL),
        '#options' => array(
          CAP_FIELDS_IMPORT_ALL => t('Import all fields for each profile. This will give you a complete list of information.'),
          CAP_FIELDS_IMPORT_LIST => t('Import a list of fields for each profile. This will allow you to select the fields that will be imported.'),
        ),
      );

      $form['synchronization']['advanced'] = array(
        '#type' => 'fieldset',
        '#title' => t('Advanced'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      $form['synchronization']['advanced']['description_wrapper'] = array(
        '#type' => 'container',
      );
      $form['synchronization']['advanced']['description_wrapper']['description'] = array(
        '#markup' => t('You can select additional options that control how specific fields will be handled during synchronization.'),
      );

      $form['synchronization']['advanced']['stanford_cap_api_import_fields_html'] = array(
        '#type' => 'checkbox',
        '#title' => t('Import HTML / Plain text fields as a single field.'),
        '#description' => t('Check this box to limit the number of fields being imported by combining the contents of HTML / Plain text fields when they are being synchronized.'),
        '#default_value' => variable_get('stanford_cap_api_import_fields_html', TRUE),
      );

      $form['actions']['submit']['#value'] = t('Next');
    }
  }
  else {
    $form['synchronization'] = array(
      '#type' => 'fieldset',
      '#title' => t('Profile Editing'),
    );

    $form['synchronization']['description'] = array(
      '#type' => 'item',
      '#markup' => t('These options allow authorized users to change the structure and content of CAP profiles. Please note, any changes to profiles will be specific to this site, and will not make their way back up to CAP.'),
    );

    $form['synchronization']['stanford_cap_api_profile_custom_fields'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow user to create custom fields?'),
      '#description' => t('Check this box to allow users to add / edit / modify fields that are part of faculty profiles. This will allow administrators to customize the information that is stored as part of each CAP profile.'),
      '#default_value' => variable_get('stanford_cap_api_profile_custom_fields', FALSE),
    );

    $form['synchronization']['stanford_cap_api_profile_allow_edit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow user to edit CAP profile information?'),
      '#description' => t('Check this box to allow users to edit the information contained in a CAP profile. Editing a profile will cause information to be changed for profiles displayed on this website only, and changes will not be synchronized back up to the CAP system.'),
      '#default_value' => variable_get('stanford_cap_api_profile_allow_edit', FALSE),
    );

    $form['refresh'] = array(
      '#type' => 'fieldset',
      '#title' => t('Refresh Options'),
    );

    $form['refresh']['stanford_cap_api_profile_update'] = array(
      '#type' => 'radios',
      '#title' => t('CAP Profiles can be updated regularly to ensure you have the latest content available for the site:'),
      '#default_value' => variable_get('stanford_cap_api_profile_update', 'nightly'),
      '#options' => array(
        // @todo Replace with named constants.
        'viewing' => t('When viewing the profile.'),
        'nightly' => t('Nightly.'),
      ),
    );

    $form['orphaned'] = array(
      '#type' => 'fieldset',
      '#title' => t('Orphaned profiles'),
    );

    $options = array(
      'unpublish' => t('Put the profile in an unpublished state.'),
      'nothing' => t('Do nothing. The profile will simply continue to exist on the website.'),
      'remove' => t('Remove it from the site.'),
    );
    $form['orphaned']['stanford_cap_api_profiles_orphaned_action'] = array(
      '#type' => 'radios',
      '#title' => t('When a profile is removed from the CAP API:'),
      '#default_value' => variable_get('stanford_cap_api_profiles_orphaned_action', 'unpublish'),
      '#options' => $options,
    );
  }

  $form['#submit'][25] = 'stanford_cap_api_profiles_settings_form_submit';
  ksort($form['#submit']);
}

/**
 * Submit handler for settings form.
 */
function stanford_cap_api_profiles_settings_form_submit($form, &$form_state) {
  $values =& $form_state['values'];
  // Field schema not synced yet.
  if (!variable_get('stanford_cap_api_profiles_schema_synchronized', FALSE)) {
    if (isset($values['stanford_cap_api_import_profile_fields'])) {
      // Initial form submit.
      variable_set('stanford_cap_api_import_profile_fields', $values['stanford_cap_api_import_profile_fields']);
      variable_set('stanford_cap_api_import_fields_html', $values['stanford_cap_api_import_fields_html']);
      if ($values['stanford_cap_api_import_profile_fields'] == CAP_FIELDS_IMPORT_LIST && !isset($values['fields'])) {
        // We need to select fields to sync.
        $form_state['rebuild'] = TRUE;
      }
      else {
        stanford_cap_api_profiles_synchronize_schema(TRUE);
      }
    }
    // Submitted after rebuild.
    else {
      _stanford_cap_api_profiles_store_sync_field_list($values['fields']);
      stanford_cap_api_profiles_synchronize_schema(TRUE);
    }
  }
  else {
    variable_set('stanford_cap_api_profile_custom_fields', $values['stanford_cap_api_profile_custom_fields']);
    variable_set('stanford_cap_api_profile_allow_edit', $values['stanford_cap_api_profile_allow_edit']);
    variable_set('stanford_cap_api_profiles_orphaned_action', $values['stanford_cap_api_profiles_orphaned_action']);
    variable_set('stanford_cap_api_profile_update', $values['stanford_cap_api_profile_update']);
  }
}

/**
 * Form builder for import page.
 */
function stanford_cap_api_profiles_import_form($form, &$form_state) {
  drupal_set_title(t('Profile import'));

  $form['description'] = array(
    '#type' => 'item',
    '#markup' => t('Individual CAP Profiles can be imported into the system by identifying the correct faculty members to add to your website. Use the tools below to search for  individual profiles, and you can select specific profiles for import   on the next screen.'),
  );

  $form['header'] = array(
    '#prefix' => '<h3>',
    '#markup' => t('Import CAP profiles'),
    '#suffix' => '</h3>',
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name / Last name'),
    '#autocomplete_path' => 'stanford-cap-api-user-autocomplete',
    '#description' => t('Please enter a name to search for. You can search using a partial match, or by selecting from the list of suggested names.'),
  );

  $link = l(t('by clicking here'), 'http://www.stanford.edu/dept/pres-provost/budget/org/orgchart/');
  $form['organization'] = array(
    '#type' => 'textfield',
    '#title' => t('Organization'),
    '#description' => t('Please select the name of the organization you wish to import. You can learn more about Stanford University Org Code Hierarchy !here.', array('!here' => $link)),
    '#default_value' => '',
    '#autocomplete_path' => 'stanford-cap-api-org-autocomplete',
  );

  $link = l(t('at the workgroup manager website.'), 'https://workgroup.stanford.edu');
  $form['workgroup'] = array(
    '#type' => 'textfield',
    '#title' => t('Workgroup'),
    '#description' => t('Please enter the name of the workgroup you wish to import. You can learn more about workgroups at Stanford, and get proper names for import, !here.', array('!here' => $link)),
  );

  $form['university_id'] = array(
    '#type' => 'textfield',
    '#title' => t('University ID'),
    '#description' => t('Please enter the university id for the profile you wish to import.'),
  );

  $form['sunet_id'] = array(
    '#type' => 'textarea',
    '#title' => t('SUNet ID'),
    '#description' => t('Please enter SUNET ids for the profiles you wish to import. You can enter multiple SUNET IDs by separating them with a comma.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  return $form;
}

/**
 * Submit handler for import form.
 */
function stanford_cap_api_profiles_import_form_submit($form, &$form_state) {
  $values = $form_state['values'];

  $params = array('ps' => CAP_PROFILES_ENTRY_PER_PAGE);

  if (!empty($values['name'])) {
    $params['name'] = check_plain($values['name']);
  }
  if (!empty($values['organization'])) {
    $term_wrapper = entity_metadata_wrapper('taxonomy_term', _stanford_cap_api_profiles_get_term_tid($values['organization']));
    $org_codes = $term_wrapper->field_org_codes->value();
    $params['orgCodes'] = implode(',', $org_codes);
  }
  if (!empty($values['workgroup'])) {
    $params['privGroups'] = _stanford_cap_api_profiles_prepare_list_str($values['workgroup']);
  }
  if (!empty($values['university_id'])) {
    $params['universityId'] = check_plain($values['university_id']);
  }
  if (!empty($values['school'])) {
    $params['orgAlias'] = check_plain($values['school']);
  }
  if (!empty($values['sunet_id'])) {
    $params['uids'] = _stanford_cap_api_profiles_prepare_list_str($values['sunet_id']);
  }

  $_SESSION['search_params'] = $params;
  $_SESSION['search_response'] = stanford_cap_api_request('/profiles/v1', $params);

  $form_state['redirect'] = 'admin/config/cap/config/import/result';
}

/**
 * Helper to prepare list for search.
 *
 * Input string could contain white space or special chars which not good for
 * API call.
 *
 * @param string $str
 *   String containing list.
 *
 * @return string
 *   Prepared string.
 */
function _stanford_cap_api_profiles_prepare_list_str($str) {
  // Replace all Windows, Unix and Mac newlines by commas.
  $str = str_replace(array("\r\n", "\r", "\n"), ',', $str);
  $list = explode(',', $str);
  array_walk($list, 'check_plain');
  $list = array_map('trim', $list);

  return implode(',', $list);
}

/**
 * Form builder for import result page.
 */
function stanford_cap_api_profiles_import_result_form($form, &$form_state) {
  drupal_set_title(t('Profile search results'));

  if (!empty($_SESSION['search_response']) && !empty($_SESSION['search_response']['values'])) {
    $form['description_wrapper'] = array(
      '#type' => 'container',
    );
    $link = l(t('Start new search.'), 'admin/config/cap/config/import');
    $form['description_wrapper']['description'] = array(
      '#markup' => t('Please select the CAP profiles you would like to import into your site. !link', array('!link' => $link)),
    );

    $form['results_wrapper'] = array(
      '#type' => 'container',
    );

    if (isset($_GET['page']) && is_numeric($_GET['page'])) {
      $page_num = (int) $_GET['page'] + 1;
    }
    else {
      $page_num = 1;
    }
    $search_params = $_SESSION['search_params'];
    if ($page_num != 1) {
      $search_params['p'] = $page_num;
      $_SESSION['search_response'] = stanford_cap_api_request('/profiles/v1', $search_params);
      $search_response = $_SESSION['search_response'];
    }
    else {
      if ($_SESSION['search_response']['page'] == $page_num) {
        $search_response = $_SESSION['search_response'];
      }
      else {
        $search_params['p'] = $page_num;
        $_SESSION['search_response'] = stanford_cap_api_request('/profiles/v1', $search_params);
        $search_response = $_SESSION['search_response'];
      }
    }

    $start = $search_response['firstPage'] ? 1 : CAP_PROFILES_ENTRY_PER_PAGE * ($search_response['page'] - 1) + 1;
    $end = $search_response['lastPage'] ? $search_response['totalCount'] : CAP_PROFILES_ENTRY_PER_PAGE * $search_response['page'];
    $total = $search_response['totalCount'];

    $form['results_wrapper']['results'] = array(
      '#markup' => t('Viewing profiles @start - @end of @total.', array(
        '@start' => $start,
        '@end' => $end,
        '@total' => $total,
      )),
    );

    $header = array(
      'name' => t('Name'),
      'link' => t('Profile link'),
    );
    $options = array();
    foreach ($search_response['values'] as $profile) {
      $href = _stanford_cap_api_profiles_get_cap_profile_url($profile);

      $options[$profile['profileId']] = array(
        'name' => array(
          'data' => array(
            '#type' => 'link',
            '#title' => check_plain($profile['displayName']),
            '#href' => $href,
          ),
        ),
        'link' => array(
          'data' => array(
            '#type' => 'link',
            '#title' => t('View in CAP'),
            '#href' => $href,
          ),
        ),
      );
    }
    $form['profiles'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
    );
    $element = 0;
    $parameters = array();
    $quantity = 9;
    global $pager_page_array, $pager_total;
    $pager_page_array = array($element => $page_num - 1);
    $pager_total = array($element => $search_response['totalPages']);

    $form['pager'] = array(
      '#markup' => theme_pager(array(
        'tags' => array(),
        'element' => $element,
        'parameters' => $parameters,
        'quantity' => $quantity,
      )),
    );

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Import profiles'),
    );
  }
  else {
    $form['description_wrapper'] = array(
      '#type' => 'container',
    );
    $link = l(t('Want to try again?'), 'admin/config/cap/config/import');
    $form['description_wrapper']['description'] = array(
      '#markup' => t("Search has returned no results. !link", array('!link' => $link)),
    );
  }

  return $form;
}

/**
 * Submit handler for import result form.
 */
function stanford_cap_api_profiles_import_result_form_submit($form, &$form_state) {

  if (empty($form_state['values']['profiles'])) {
    drupal_set_message(t('Nothing to import. Profiles list is empty.'));
  }
  else {
    if (!variable_get('stanford_cap_api_profiles_schema_synchronized', FALSE)) {
      stanford_cap_api_profiles_synchronize_schema(TRUE);
    }
    $profiles = $form_state['values']['profiles'];
    $operations = array();
    foreach ($profiles as $profile) {
      if ($profile != 0) {
        $operations[] = array(
          'stanford_cap_api_profiles_profile_import',
          array($profile),
        );
      }
    }

    $batch = array(
      'title' => t('Importing profiles'),
      'operations' => $operations,
      'progress_message' => t('Current: @current | Remaining: @remaining | Total: @total | Estimate: @estimate | Elapsed: @elapsed'),
      'finished' => 'stanford_cap_api_profiles_import_result_finished',
    );
    batch_set($batch);
  }
}

/**
 * Remove all CAP profile fields from system.
 */
function stanford_cap_api_profiles_reset() {
  $fields = db_select('cap_api_fields', 'f')->fields('f', array('field_name'))
    ->execute()->fetchCol();
  $operations = array();
  foreach ($fields as $field) {
    if (field_read_field($field)) {
      $operations[] = array(
        'field_delete_field',
        array($field),
      );
      $operations[] = array(
        'field_purge_batch',
        array(1),
      );
    }
  }
  batch_set(array('operations' => $operations));
  db_delete('cap_api_fields')->execute();
  db_delete('cap_api_field_hashes')->execute();
  db_delete('cap_api_fields_map')->execute();
  variable_del('stanford_cap_api_profiles_schema_synchronized');
  variable_del('stanford_cap_api_profiles_schema_hash');
  drupal_set_message(t('Stanford profile content type has being resetted to default.'));
}

/**
 * Groups synchronization from builder.
 */
function stanford_cap_api_profiles_group_sync_form($form, &$form_state) {
  drupal_set_title(t('Group import'));

  $form['description'] = array(
    '#type' => 'item',
    '#markup' => t('CAP profiles associated with specific groups can be automatically synced with this site on an ongoing basis. Use the tools on this page to manage the groups that are synced with the site.'),
  );

  if (empty($form_state['values'])) {

    $form['groups'] = array(
      '#type' => 'fieldset',
      '#title' => t('Groups being synced'),
    );

    $groups = db_select('cap_api_sync_groups', 'groups')
      ->fields('groups')
      ->execute()
      ->fetchAll();
    $header = array(
      t('Name'),
      t('Profiles synchronized'),
      t('Last synced date'),
      t('Operations'),
    );
    $rows = array();
    foreach ($groups as $group) {
      $delete_link = l(t('Remove'), 'admin/config/cap/config/group/' . $group->id . '/delete');
      $sync_link = l(t('Sync now'), 'admin/config/cap/config/group/' . $group->id . '/sync');
      $rows[] = array(
        check_plain($group->name),
        t('%count profiles', array('%count' => $group->count)),
        $group->lastsynced ? format_date($group->lastsynced, 'long') : t('Never'),
        $delete_link . ' | ' . $sync_link,
      );
    }

    $table = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'empty' => t('No groups were saved for synchronization yet.'),
    ));

    $form['groups']['groups_list'] = array(
      '#markup' => $table,
    );

    $form['new_group'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add new group'),
    );

    $link = l(t('by clicking here'), 'http://www.stanford.edu/dept/pres-provost/budget/org/orgchart/');
    $form['new_group']['organization'] = array(
      '#type' => 'textfield',
      '#title' => t('Organization'),
      '#description' => t('Please select the name of the organization you wish to import. You can learn more about Stanford University Org Code Hierarchy !here.', array('!here' => $link)),
      '#default_value' => '',
      '#autocomplete_path' => 'stanford-cap-api-org-autocomplete',
    );

    $link = l(t('at the workgroup manager website'), 'https://workgroup.stanford.edu');
    $form['new_group']['workgroup'] = array(
      '#type' => 'textfield',
      '#title' => t('Workgroup'),
      '#description' => t('Please enter the name of the workgroup you wish to import. You can learn more about workgroups at Stanford, and get proper names for import, !here.', array('!here' => $link)),
    );

    $form['new_group']['search'] = array(
      '#type' => 'submit',
      '#value' => t('Search'),
    );
  }
  else {
    $form['groups'] = array(
      '#type' => 'fieldset',
      '#title' => t('Search results'),
    );

    $header = array(
      'name' => t('Group'),
      'profiles' => t('Profiles in group'),
    );
    $options = array();
    $group_types = array('organization', 'workgroup');
    $form['groups_to_save'] = array(
      '#type' => 'container',
      '#tree' => TRUE,
    );

    foreach ($group_types as $group_type) {
      switch ($group_type) {
        case 'organization':
          $tid = _stanford_cap_api_profiles_get_term_tid($form_state['values'][$group_type]);
          if ($tid) {
            $term_wrapper = entity_metadata_wrapper('taxonomy_term', $tid);
            $org_codes = $term_wrapper->field_org_codes->value();
          }
          $name = '';
          if (!empty($org_codes)) {
            $name = implode(',', $org_codes);
          }
          break;

        default:
          $name = check_plain(trim($form_state['values'][$group_type]));
          break;
      }
      if (!empty($name)) {
        $count = _stanford_cap_api_profiles_get_group_profiles_count($group_type, $name);
        $label = check_plain($name);
        if ($group_type == 'organization') {
          $label = $term_wrapper->name->value();
        }
        $options[$group_type] = array('name' => $label, 'profiles' => $count);
        $form['groups_to_save'][$group_type] = array(
          '#type' => 'value',
          '#value' => $name,
        );
      }
    }

    $form['groups']['groups_list'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
    );

    $form['groups']['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save selected groups'),
    );
  }

  return $form;
}

/**
 * Groups synchronization from submit handler.
 */
function stanford_cap_api_profiles_group_sync_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['organization'][0]) || !empty($form_state['values']['workgroup']) || !empty($form_state['values']['school'])) {
    $form_state['rebuild'] = TRUE;
  }
  if (isset($form_state['values']['groups_list'])) {
    $operations = array();
    foreach ($form_state['values']['groups_list'] as $group_type) {
      if (isset($form_state['values']['groups_to_save'][$group_type])) {
        $group_name = $form_state['values']['groups_to_save'][$group_type];
        $count = 0;
        $operations[] = array(
          'stanford_cap_api_profiles_sync_group_save',
          array($group_type, $group_name, NULL, NULL),
        );
        switch ($group_type) {
          case 'organization':
            $count = stanford_cap_api_profiles_sync_org_members($group_name, $operations);
            break;

          case 'workgroup':
            $count = stanford_cap_api_profiles_sync_workgroup_members($group_name, $operations);
            break;
        }
        $operations[] = array(
          '_stanford_cap_api_profiles_sync_group_save_wrapper',
          array($group_type, $group_name, $count),
        );
      }
    }
    $batch = array(
      'title' => t('Synchronizing group profiles'),
      'operations' => $operations,
      'progress_message' => t('Current: @current | Remaining: @remaining | Total: @total | Estimate: @estimate | Elapsed: @elapsed'),
    );
    batch_set($batch);
  }
}

/**
 * Helper to get group profiles count.
 *
 * @param string $type
 *   Group type.
 * @param string $name
 *   Group name.
 *
 * @return int
 *   Number of profiles in this group.
 */
function _stanford_cap_api_profiles_get_group_profiles_count($type, $name) {
  $count = 0;
  switch ($type) {
    case 'organization':
      $params = array('orgCodes' => $name);
      $data = stanford_cap_api_request('/profiles/v1', $params);
      if ($data) {
        $count = $data['totalCount'];
      }
      break;

    case 'workgroup':
      $params = array('privGroups' => $name);
      $data = stanford_cap_api_request('/profiles/v1', $params);
      if ($data) {
        $count = $data['totalCount'];
      }
      break;

    case 'school':
      break;
  }

  return $count;
}

/**
 * Drop profiles group from synchronization form builder.
 */
function stanford_cap_api_profiles_group_sync_drop_form($form, &$form_state, $id) {
  $form_addition = array('id' => array('#type' => 'value', '#value' => $id));
  $question = t('Are you sure you want to stop syncing this group?');

  return confirm_form($form_addition, $question, 'admin/config/cap/config/group', NULL, t('Delete'));
}

/**
 * Drop profiles group from synchronization form submit handler..
 */
function stanford_cap_api_profiles_group_sync_drop_form_submit($form, &$form_state) {
  stanford_cap_api_profiles_sync_group_drop($form_state['values']['id']);
  $form_state['redirect'] = 'admin/config/cap/config/group';
  drupal_set_message(t('Group was successfully deleted.'));
}

/**
 * Starts profiles group synchronization.
 */
function stanford_cap_api_profiles_group_sync_now($id) {
  $group = db_select('cap_api_sync_groups', 'groups')
    ->condition('id', $id)
    ->fields('groups')
    ->execute()
    ->fetchAssoc();
  $operations = array();
  $count = 0;
  $operations[] = array(
    'stanford_cap_api_profiles_sync_group_save',
    array($group['type'], $group['name'], NULL, NULL),
  );
  switch ($group['type']) {
    case 'organization':
      $count = stanford_cap_api_profiles_sync_org_members($group['name'], $operations);
      break;

    case 'workgroup':
      $count = stanford_cap_api_profiles_sync_workgroup_members($group['name'], $operations);
      break;
  }
  $operations[] = array(
    '_stanford_cap_api_profiles_sync_group_save_wrapper',
    array($group['type'], $group['name'], $count),
  );

  $batch = array(
    'title' => t('Synchronizing group profiles'),
    'operations' => $operations,
    'progress_message' => t('Current: @current | Remaining: @remaining | Total: @total | Estimate: @estimate | Elapsed: @elapsed'),
  );
  batch_set($batch);
  batch_process('admin/config/cap/config/group');
}

/**
 * Menu callback for CAP profiles name autocomplete.
 *
 * @param string $str
 *   String to search for.
 */
function stanford_cap_api_profiles_user_autocomplete($str = '') {
  $matches = array();
  if ($str) {
    $response = stanford_cap_api_request('/cap/v1/search/autocomplete', array('q' => $str));
    if (!empty($response['count'])) {
      foreach ($response['values'] as $match) {
        $matches[$match['name']] = $match['name'];
      }
    }
  }

  drupal_json_output($matches);
}

/**
 * Autocomplete callback for organization taxonomy search.
 *
 * @param string $tag_typed
 *   Typed search string.
 *
 * @see taxonomy_autocomplete()
 */
function stanford_cap_api_profiles_taxonomy_autocomplete($tag_typed = '') {
  $voc = taxonomy_vocabulary_machine_name_load(CAP_ORGS_VOCAB);
  $query = db_select('taxonomy_term_data', 't');
  $query->addTag('translatable');
  $query->addTag('term_access');

  // Do not select already entered terms.
  if (!empty($tags_typed)) {
    $query->condition('t.name', $tags_typed, 'NOT IN');
  }
  // Select rows that match by term name.
  $tags_return = $query
    ->fields('t', array('tid', 'name'))
    ->condition('t.vid', $voc->vid)
    ->condition('t.name', '%' . db_like($tag_typed) . '%', 'LIKE')
    ->range(0, 10)
    ->execute()
    ->fetchAllKeyed();

  $term_matches = array();

  foreach ($tags_return as $tid => $name) {
    $term_matches[$name . ' (' . $tid . ')'] = check_plain($name);
  }

  drupal_json_output($term_matches);
}

/**
 * Get term tid from composite term name.
 *
 * @param string $str
 *   Composite term name. Term name with added " (TID)".
 *
 * @return int
 *   Term tid or FALSE.
 */
function _stanford_cap_api_profiles_get_term_tid($str) {
  $matches = array();
  if (preg_match('/^.*\ \((\d+)\)$/', $str, $matches)) {
    // Term tid captured by first capture group.
    return $matches[1];
  }

  return FALSE;
}

/**
 * Creates FAPI checkboxes for fields.
 */
function _stanford_cap_api_profiles_field_checkboxes($fields, &$parent_elem, $parent_field = '') {
  $weight =& drupal_static(__FUNCTION__, -100);

  foreach ($fields as $f_name => $f_schema) {
    // Skip field collection with empty properties.
    if ($f_schema['type'] == 'object' && empty($f_schema['properties'])) {
      continue;
    }
    $field_path = empty($parent_field) ? $f_name : $parent_field . '.' . $f_name;
    $field_level = count(explode('.', $field_path));
    $parent_elem[$field_path] = array(
      '#type' => 'checkbox',
      '#title' => empty($f_schema['title']) ? $f_name : check_plain($f_schema['title']),
      '#description' => empty($f_schema['description']) ? '' : check_plain($f_schema['description']),
      '#return_value' => $field_path,
      '#attributes' => array(
        'parent' => $parent_field,
        'class' => array('level-' . $field_level),
      ),
    );

    // Field collection field which will be merged to single string field.
    $merge_field = (variable_get('stanford_cap_api_import_fields_html', TRUE) && $f_schema['type'] == 'object' && !empty($f_schema['properties']) && isset($f_schema['properties']['html']) && isset($f_schema['properties']['text']) && count($f_schema['properties']));
    // Lets move simple fields upper in list.
    if (empty($parent_field)
      && ($f_schema['type'] == 'string'
      || ($f_schema['type'] == 'array' && $f_schema['items']['type'] == 'string')
      || $merge_field)) {
      $parent_elem[$field_path]['#weight'] = $weight;
      $weight++;
    }
    // We do not want to show subfields if field merging was selected.
    if ($merge_field) {
      unset($f_schema['properties']);
    }

    $subfields = array();
    if ($f_schema['type'] == 'array' && $f_schema['items']['type'] == 'object') {
      $subfields = $f_schema['items']['properties'];
    }
    elseif ($f_schema['type'] == 'object' && !empty($f_schema['properties'])) {
      $subfields = $f_schema['properties'];
    }
    if (!empty($subfields)) {
      _stanford_cap_api_profiles_field_checkboxes($subfields, $parent_elem, $field_path);
    }
  }

}

/**
 * Stores array of fields to be synchronized in variable.
 *
 * Adds a list of orgCode fields to provided list.
 */
function _stanford_cap_api_profiles_store_sync_field_list($fields) {
  $field_list = array_filter($fields);
  $orgcode_fields = stanford_cap_api_profiles_get_orgcode_fields();
  if (!empty($orgcode_fields)) {
    $field_list = array_merge($field_list, $orgcode_fields);
  }
  variable_set('stanford_cap_api_profiles_sync_field_list', $field_list);
}
