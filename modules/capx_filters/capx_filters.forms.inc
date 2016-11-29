<?php

/**
 * @file
 * CAPx Filters forms.
 */

use \CAPx\Drupal\Util\CAPx;


/**
 * Menu Callback to display current filters active on the CAPx mapper.
 *
 * @param string $mapper_id
 *   Machine name of the capx mapper.
 *
 * @return array
 *   Render array of the table.
 */
function capx_filters_list($mapper_id) {
  $mapper = capx_cfe_load_by_machine_name($mapper_id, 'mapper');
  // Mapper Not Found.
  if (!$mapper) {
    drupal_not_found();
    drupal_exit();
  }


  $filters = array();
  if (isset($mapper->settings['filters'])) {
    foreach ($mapper->settings['filters'] as $key => $filter) {
      $actions = l(t('Edit'), CAPX_FILTERS_PAGE . "/$mapper_id/filters/$key/edit");
      $actions .= ' | ';
      $actions .= l(t('Delete'), CAPX_FILTERS_PAGE . "/$mapper_id/filters/$key/delete");
      $filters[] = array(
        'key' => $filter['key'],
        'comparison' => $filter['comparison'],
        'value' => $filter['value'],
        'exclude' => $filter['exclude'] ? 'Exclude' : 'Include',
        'actions' => $actions,
      );
    }
  }
  $output['menu'] = stanford_capx_management_menu_block();
  $output['table'] = array(
    '#theme' => 'table',
    '#header' => array(
      t('Path'),
      t('Comparison'),
      t('Value'),
      t('Include/Exclude'),
      t('Actions'),
    ),
    '#rows' => $filters,
    '#empty' => t('No Filters Configured'),
  );

  return $output;
}

/**
 * Form to create or edit a filter for the given CAPx Mapper.
 */
function capx_filters_edit($form, &$form_state, $mapper_id, $filter_id = -1) {
  $key = '';
  $value = '';
  $comparison = '=';
  $exclude = 0;
  $mapper = capx_cfe_load_by_machine_name($mapper_id, 'mapper');
  // Mapper Not Found.
  if (!$mapper) {
    drupal_not_found();
    drupal_exit();
  }

  if ($filter_id !== -1) {
    // Filter ID was given, but check to see if it actually exists.
    if (isset($mapper->settings['filters']) && isset($mapper->settings['filters'][$filter_id])) {
      $filter = $mapper->settings['filters'][$filter_id];
      $key = $filter['key'];
      $value = $filter['value'];
      $comparison = $filter['comparison'];
      $exclude = $filter['exclude'];
    }
    else {
      drupal_not_found();
      drupal_exit();
    }
  }
  $form = array();

  $form['mapper_id'] = array(
    '#type' => 'hidden',
    '#value' => $mapper_id,
  );
  $form['filter_id'] = array(
    '#type' => 'hidden',
    '#value' => $filter_id,
  );
  $form['key'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to value'),
    '#default_value' => $key,
    '#required' => TRUE,
  );
  $form['value'] = array(
    '#type' => 'textfield',
    '#title' => t('Value to check'),
    '#default_value' => $value,
    '#required' => TRUE,
  );
  $form['comparison'] = array(
    '#type' => 'select',
    '#title' => t('Comparison operation'),
    '#options' => array(
      '=' => '=',
      '>' => '>',
      '<' => '<',
      'contains' => t('Contains'),
    ),
    '#default_value' => $comparison,
    '#required' => TRUE,
  );
  $form['exclude'] = array(
    '#type' => 'checkbox',
    '#title' => t('Exclude content with this value'),
    '#default_value' => $exclude,
  );
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Saves the filter to the given CAPx Mapper.
 */
function capx_filters_edit_submit($form, &$form_state) {
  $mapper = capx_cfe_load_by_machine_name($form_state['values']['mapper_id'], 'mapper');
  $filter = array(
    'key' => $form_state['values']['key'],
    'value' => $form_state['values']['value'],
    'comparison' => $form_state['values']['comparison'],
    'exclude' => $form_state['values']['exclude'],
  );
  if (!isset($mapper->settings['filters'])) {
    $mapper->settings['filters'] = array();
  }

  if ($form_state['values']['filter_id'] !== -1) {
    $mapper->settings['filters'][$form_state['values']['filter_id']] = $filter;
  }
  else {
    array_push($mapper->settings['filters'], $filter);
  }

  $mapper->settings['filters'] = array_values($mapper->settings['filters']);
  capx_mapper_save($mapper);
  CAPx::invalidateEtags('mapper', $mapper);
  $form_state['redirect'] = CAPX_FILTERS_PAGE . "/{$mapper->getMachineName()}/filters";
}

/**
 * Form to confirm the deletion of the filter.
 */
function capx_filters_delete($form, &$form_state, $mapper_id, $filter_id) {
  // Mapper Not Found.
  if (!capx_cfe_load_by_machine_name($mapper_id, 'mapper')) {
    drupal_not_found();
    drupal_exit();
  }

  $form = array();
  $form['disclaimer'] = array(
    '#markup' => t('Are you sure you wish to delete this filter? This action cannot be undone'),
  );
  $form['mapper_id'] = array(
    '#type' => 'hidden',
    '#value' => $mapper_id,
  );
  $form['filter_id'] = array(
    '#type' => 'hidden',
    '#value' => $filter_id,
  );
  $form['actions'] = array(
    '#type' => 'actions',
  );

  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#name' => 'delete',
    '#value' => t('Delete'),
  );

  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#name' => 'cancel',
    '#value' => t('Cancel'),
  );

  return $form;
}

/**
 * Removes the filter from the CAPx mapper settings array.
 */
function capx_filters_delete_submit($form, &$form_state) {
  global $_CAPX_FILTERS_PAGE_;
  $mapper_id = $form_state['values']['mapper_id'];
  $form_state['redirect'] = CAPX_FILTERS_PAGE . "/$mapper_id/filters";
  if ($form_state['triggering_element']['#name'] == 'cancel') {
    return;
  }
  $mapper = capx_cfe_load_by_machine_name($mapper_id, 'mapper');
  unset($mapper->settings['filters'][$form_state['values']['filter_id']]);
  $mapper->settings['filters'] = array_values($mapper->settings['filters']);
  capx_mapper_save($mapper);
  CAPx::invalidateEtags('mapper', $mapper);
}
