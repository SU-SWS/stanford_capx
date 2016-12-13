<?php
/**
 * @file
 * CAPx Tamper Forms.
 *
 * Mostly taken from feeds_tamper_ui.admin.inc.
 */

/**
 * Shows the current tampers in table format for the given CAPx Mapper.
 *
 * @param string $mapper_id
 *   Machine name of the mapper to apply tamper to.
 *
 * @return array
 *   Renderable array.
 */
function capx_tamper_list($mapper_id) {
  /** @var \CAPx\Drupal\Entities\CFEntity $mapper */
  $mapper = capx_tamper_check_mapper($mapper_id);

  drupal_set_title(t('!mapper Tamper Plugins', array('!mapper' => $mapper->title)));
  $output = array();

  $output['goback'] = array(
    "#markup" => "<p>" . l(t("Manage Mapping Configuration ->"), "admin/config/capx/mapper/edit/" . $mapper_id) . "</p>",
  );

  $header = array(
    t('Description'),
    t('Plugin'),
    t('Operations'),
    t('Enabled'),
  );
  $fields = array();
  foreach ($mapper->fields as $field_key => $field_settings) {
    $field_settings = array_filter($field_settings);
    if ($field_settings) {
      $fields[$field_key] = $field_settings[key($field_settings)];
    }
  }

  $sources = array_merge($mapper->properties, $fields);

  foreach ($sources as $field_key => $path) {
    $rows = array();

    $instance = field_info_instance($mapper->entity_type, $field_key, $mapper->bundle_type);
    $conditions = array('mapper' => $mapper_id, 'source' => $field_key);
    foreach (capx_tamper_load_tampers($conditions) as $tamper) {
      $edit_url = CAPX_TAMPER . "/{$mapper->identifier()}/tamper/{$tamper->id}/edit";
      $delete_url = CAPX_TAMPER . "/{$mapper->identifier()}/tamper/{$tamper->id}/delete";
      $plugin = feeds_tamper_get_plugin($tamper->plugin_id);
      $rows[] = array(
        $tamper->description,
        $plugin['name'],
        l(t('Edit'), $edit_url) . ' | ' . l(t('Delete'), $delete_url),
        '&#x2705',
      );
    }

    if (!$rows) {
      $rows[] = array(
        array(
          'data' => t('No plugins defined'),
          'colspan' => count($header),
        ),
      );
    }

    $rows[] = array(
      array(
        'data' => l(t('Add Plugin'), CAPX_TAMPER . "/$mapper_id/tamper/add/$field_key"),
        'colspan' => count($header),
      ),
    );
    $output[$field_key] = array(
      '#theme' => 'table',
      '#caption' => t('!from -> !to', array(
        '!from' => $path,
        '!to' => $instance ? $instance['label'] : $field_key,
      )),
      '#header' => $header,
      '#rows' => $rows,
    );

  }

  return $output;
}

/**
 * Tamper add form.
 */
function capx_tamper_add($form, &$form_state, $mapper_id, $source) {
  drupal_set_title(t('Add Tamper Plugin'));
  /** @var \CAPx\Drupal\Entities\CFEntity $mapper */
  $mapper = capx_tamper_check_mapper($mapper_id);

  $form_state['mapper'] = $mapper;
  $form_state['source'] = $source;
  $form['#attached']['css'][] = drupal_get_path('module', 'capx_tamper') . '/css/capx_tamper.css';

  // Build plugin select list.
  $capx_tamper_plugins = feeds_tamper_get_plugins();
  $plugins = array();
  foreach ($capx_tamper_plugins as $plugin_id => $plugin) {
    $plugins[t($plugin['category'])][$plugin_id] = t($plugin['name']);
  }
  ksort($plugins);
  foreach ($plugins as &$p) {
    asort($p);
  }

  $machine_name = key(reset($plugins));
  if (!empty($form_state['values']['plugin_id'])) {
    $machine_name = $form_state['values']['plugin_id'];
  }
  $plugin = feeds_tamper_get_plugin($machine_name);

  $form['plugin_id'] = array(
    '#title' => t('The plugin to add'),
    '#type' => 'select',
    '#options' => $plugins,
    '#default_value' => '',
    '#tree' => TRUE,
    '#ajax' => array(
      'callback' => 'capx_tamper_ajax_callback',
      'wrapper' => 'capx-tamper-plugin',
      'progress' => 'none',
    ),
  );
  $form['update'] = array(
    '#type' => 'submit',
    '#limit_validation_errors' => array(array('plugin_id')),
    '#submit' => array('capx_tamper_ui_add_plugin_form_submit'),
    '#value' => t('Choose'),
    '#attributes' => array('class' => array('no-js')),
  );
  $form['plugin']['#prefix'] = '<div class="clear-fix" id="capx-tamper-plugin">';
  $form['plugin']['#suffix'] = '</div>';

  $form['plugin']['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#default_value' => $plugin['default description'] ? t($plugin['default description']) : t($plugin['name']),
    '#required' => TRUE,
    '#description' => t('A useful description of what this plugin is doing.'),
  );
  $form['plugin']['id'] = array(
    '#title' => t('Machine name'),
    '#type' => 'machine_name',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'capx_tamper_machine_name_callback',
      'source' => array('plugin', 'description'),
    ),
    '#default_value' => $machine_name,
  );
  $form['plugin']['settings'] = array(
    '#title' => t('Configure @name', array('@name' => $plugin['name'])),
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );
  $importer = new stdClass();

  $form['plugin']['settings'] += $plugin['form']($importer, $source, array(), $form_state);
  $form['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

/**
 * Tamper add form validation
 */
function capx_tamper_add_validate($form, &$form_state) {
  if ($form_state['triggering_element']['#value'] == t('Add')) {
    if (capx_tamper_machine_name_callback($form_state['values']['id'], $form, $form_state)) {
      form_set_error('id', t('The machine-readable name is already in use. It must be unique.'));
      return;
    }
    $plugin_id = $form_state['values']['plugin_id'];
    $plugin = feeds_tamper_get_plugin($plugin_id);
    if ($plugin['validate'] && isset($form_state['values']['settings'])) {
      $plugin['validate']($form_state['values']['settings']);
    }
    return;
  }
  unset($form_state['input']['id']);
  unset($form_state['input']['description']);
  unset($form_state['input']['settings']);
}

/**
 * Tamper add form submit to save to table.
 */
function capx_tamper_add_submit($form, &$form_state) {
  if ($form_state['triggering_element']['#value'] == t('Add')) {
    /** @var \CAPx\Drupal\Entities\CFEntity $mapper */
    $mapper = $form_state['mapper'];
    $id = implode('-', array(
      $mapper->identifier(),
      $form_state['source'],
      $form_state['values']['id'],
    ));

    $tamper = new stdClass();
    $tamper->id = $id;
    $tamper->mapper = $mapper->identifier();
    $tamper->source = $form_state['source'];
    $tamper->plugin_id = $form_state['values']['plugin_id'];
    $tamper->settings = $form_state['values']['settings'];
    $tamper->weight = '0';
    $tamper->description = $form_state['values']['description'];


    capx_tamper_save_instance($tamper);
    $form_state['redirect'] = CAPX_TAMPER . "/{$mapper->identifier()}/tamper";

    drupal_set_message(t('Plugin %description was successfully added to %source.', array(
      '%description' => $form_state['values']['description'],
      '%source' => $form_state['source'],
    )));
    return;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Checks for existing machine name.
 */
function capx_tamper_machine_name_callback($id, $form, &$form_state) {
  $id = implode('-', array(
    $form_state['mapper']->identifier(),
    $form_state['source'],
    $form_state['values']['id'],
  ));
  return capx_tamper_load_tampers(array('id' => $id));
}


/**
 * Ajax callback for add plugin form.
 */
function capx_tamper_ajax_callback($form, $form_state) {
  return $form['plugin'];
}

/**
 * Confirmation of delete tamper.
 */
function capx_tamper_delete($form, &$form_state, $mapper_id, $tamper_id) {
  $mapper = capx_tamper_check_mapper($mapper_id);

  $tamper = reset(capx_tamper_load_tampers(array('id' => $tamper_id)));
  $form_state['tamper'] = $tamper;
  $form_state['mapper'] = $mapper;
  $question = t('Would you really like to delete the plugin @instance?', array('@instance' => $tamper->description));
  $button_label = t('Delete');

  return confirm_form(
    $form,
    $question,
    CAPX_TAMPER . "/{$mapper->identifier()}/tamper",
    NULL,
    $button_label
  );
}

/**
 * Tamper delete form submit.
 */
function capx_tamper_delete_submit($form, &$form_state) {
  $tamper = $form_state['tamper'];
  /** @var \CAPx\Drupal\Entities\CFEntity $mapper */
  $mapper = $form_state['mapper'];

  capx_tamper_delete_instance($tamper);

  drupal_set_message(t('The plugin %plugin has been deleted from %source.', array(
    '%plugin' => $tamper->description,
    '%source' => $tamper->source,
  )));

  $form_state['redirect'] = CAPX_TAMPER . "/{$mapper->identifier()}/tamper";
}

/**
 * Checks for valid capx mapper and throws 404 if invalid.
 */
function capx_tamper_check_mapper($mapper_id) {
  $mapper = capx_cfe_load_by_machine_name($mapper_id, 'mapper');
  if (!$mapper) {
    drupal_not_found();
    drupal_exit();
  }
  return $mapper;
}

/**
 * Tamper plugin edit form.
 */
function capx_tamper_edit($form, &$form_state, $mapper_id, $tamper_id) {
  /** @var \CAPx\Drupal\Entities\CFEntity $mapper */
  $mapper = capx_tamper_check_mapper($mapper_id);
  $form_state['mapper'] = $mapper;
  $tamper = reset(capx_tamper_load_tampers(array('id' => $tamper_id)));
  $form_state['tamper'] = $tamper;

  drupal_set_title(t('Edit !title', array('!title' => $tamper->description)));

  $plugin = feeds_tamper_get_plugin($tamper->plugin_id);
  $form['#tree'] = TRUE;

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#description' => t('A useful description of what this plugin is doing.'),
    '#default_value' => $tamper->description,
  );
  $form['settings'] = array(
    '#title' => t('Configure @plugin', array('@plugin' => $plugin['name'])),
    '#type' => 'fieldset',
    '#tree' => TRUE,
  );
  $importer = new stdClass();
  $form['settings'] += $plugin['form']($importer, $tamper->source, $tamper->settings, $form_state);

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['#attached']['css'][] = drupal_get_path('module', 'capx_tamper') . '/css/capx_tamper.css';

  return $form;
}

/**
 * Tamper edit validation.
 */
function capx_tamper_edit_validate($form, &$form_state) {
  $plugin_id = $form_state['instance']->plugin_id;
  $plugin = feeds_tamper_get_plugin($plugin_id);
  if ($plugin['validate']) {
    $plugin['validate']($form_state['values']['settings']);
  }
}

/**
 * Tamper plugin edit submit.
 */
function capx_tamper_edit_submit($form, &$form_state) {
  $mapper = $form_state['mapper'];
  $tamper = $form_state['tamper'];
  if (isset($form_state['values']['settings'])) {
    $tamper->settings = $form_state['values']['settings'];
  }
  $tamper->description = $form_state['values']['description'];

  capx_tamper_save_instance($tamper);
  drupal_set_message(t('The plugin %plugin has been updated.', array('%plugin' => $tamper->description)));
  $form_state['redirect'] = CAPX_TAMPER . "/{$mapper->identifier()}/tamper";
}