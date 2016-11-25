<?php

/**
 * @file
 * CAPx Filters forms.
 */

/**
 * CAPx Filters list page.
 *
 * @return array
 *   Render array.
 */
function capx_filters_list() {
  $output = array();
  $menu_block = stanford_capx_management_menu_block();
  $output['content']['capx_menu_block'] = $menu_block;

  $filters = array();
  foreach (entity_load('capx_cfe') as $id => $entity) {
    if ($entity->type == 'mapper' && isset($entity->settings['filters'])) {
      $filters[$id] = array(
        $entity->title,
        count($entity->settings['filters']),
        l(t('Edit'), '/admin/config/capx/filters/edit/' . $entity->machine_name) . ' | ' .
        l(t('Delete'), '/admin/config/capx/filters/delete/' . $entity->machine_name),
      );
    }
  }

  $output['content']['table'] = array(
    '#theme' => 'table',
    '#header' => array(
      t('Mapper'),
      t('Number of filters'),
      t('Actions'),
    ),
    '#rows' => $filters,
    '#empty' => t('No Filters Configured'),
  );

  return $output;
}

/**
 * CAPx Filter Edit/Add page.
 */
function capx_filters_edit($form, &$form_state, $id) {
  drupal_add_css(drupal_get_path('module', 'capx_filters') . '/css/capx_filters.css');
  $attach_to = array();
  $filters = array();
  foreach (entity_load('capx_cfe') as $entity) {
    if ($entity->type == 'mapper') {
      $attach_to[$entity->machine_name] = $entity->title;
      if ($entity->machine_name == $id && isset($entity->settings['filters'])) {
        $filters = $entity->settings['filters'];
      }
      else {
        if (isset($entity->settings['filters'])) {
          unset($attach_to[$entity->machine_name]);
        }
      }
    }
  }

  $form = array();
  if (!$attach_to) {
    drupal_set_message(t('No available mappers to attach a filter to. Please create a new mapper.'), 'warning');
  }
  $form['#tree'] = TRUE;
  $form['attach_to'] = array(
    '#type' => 'select',
    '#title' => t('Apply filters to which mapper'),
    '#options' => $attach_to,
    '#default_value' => $id,
    '#required' => TRUE,
    '#disabled' => $id || !$attach_to,
  );
  if (empty($form_state['num_filters'])) {
    if ($filters) {
      $form_state['num_filters'] = count($filters);
    }
    else {
      $form_state['num_filters'] = 1;
    }
  }
  $form['filters'] = array(
    '#type' => 'container',
    '#prefix' => '<div id="filters-wrapper">',
    '#suffix' => '</div>',
    '#attributes' => array(
      'class' => array(
        'clearfix',
      ),
    ),
    '#disabled' => $id || !$attach_to,
  );
  for ($i = 1; $i <= $form_state['num_filters']; $i++) {
    $form['filters'][$i]['container'] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array(
          'filter-item',
        ),
      ),
    );
    $form['filters'][$i]['container']['key'] = array(
      '#type' => 'textfield',
      '#title' => t('Key'),
      '#required' => TRUE,
      '#default_value' => isset($filters[$i]) ? $filters[$i]['key'] : '',
      '#disabled' => !$attach_to,
    );
    $form['filters'][$i]['container']['value'] = array(
      '#type' => 'textfield',
      '#title' => t('Value'),
      '#required' => TRUE,
      '#default_value' => isset($filters[$i]) ? $filters[$i]['value'] : '',
      '#disabled' => !$attach_to,
    );
    $form['filters'][$i]['container']['comparison'] = array(
      '#type' => 'select',
      '#title' => t('Comparison'),
      '#options' => array(
        '=' => '=',
        '>' => '>',
        '<' => '<',
        'contains' => t('Contains'),
      ),
      '#required' => TRUE,
      '#default_value' => isset($filters[$i]) ? $filters[$i]['comparison'] : '=',
      '#disabled' => !$attach_to,
    );
    $form['filters'][$i]['container']['exclude'] = array(
      '#type' => 'checkbox',
      '#title' => t('Exclude'),
      '#default_value' => isset($filters[$i]) ? $filters[$i]['exclude'] : 0,
      '#disabled' => !$attach_to,
    );
  }
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add More'),
    '#submit' => array('capx_filters_add_more'),
    '#ajax' => array(
      'callback' => 'capx_filters_add_more_callback',
      'wrapper' => 'filters-wrapper',
    ),
    '#disabled' => !$attach_to,
  );

  $form['actions']['remove_one'] = array(
    '#type' => 'submit',
    '#value' => t('Remove One'),
    '#submit' => array('capx_filters_remove_one'),
    '#ajax' => array(
      'callback' => 'capx_filters_remove_one_callback',
      'wrapper' => 'filters-wrapper',
    ),
    '#disabled' => !$attach_to,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#weight' => 999,
    '#disabled' => !$attach_to,
  );


  return $form;
}

/**
 * Ajax Submit to remove last filter.
 */
function capx_filters_remove_one($form, &$form_state) {
  $form_state['num_filters']--;
  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax callback.
 */
function capx_filters_remove_one_callback($form, &$form_state) {
  return $form['filters'];
}

/**
 * Ajax Submit to add more filters.
 */
function capx_filters_add_more($form, &$form_state) {
  $form_state['num_filters']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Ajax callback.
 */
function capx_filters_add_more_callback($form, &$form_state) {
  return $form['filters'];
}

/**
 * Filter Edit/Add submit to save filters to mapper entity.
 */
function capx_filters_edit_submit($form, &$form_state) {
  unset($form_state['values']['filters']['actions']);
  foreach ($form_state['values']['filters'] as &$filter) {
    $filter = $filter['container'];
  }

  foreach (entity_load('capx_cfe') as $mapper) {
    if ($mapper->machine_name == $form_state['values']['attach_to']) {
      $mapper->settings['filters'] = $form_state['values']['filters'];
      entity_save('capx_cfe', $mapper);
      break;
    }
  }
  $form_state['redirect'] = 'admin/config/capx/filters';
}

/**
 * Confirmation form to delete filters.
 */
function capx_filters_delete($form, &$form_state, $id) {
  $mapper = new stdClass();
  foreach (entity_load('capx_cfe') as $mapper) {
    if ($mapper->machine_name == $id) {
      break;
    }
  }
  $form = array();
  $form['id'] = array(
    '#type' => 'hidden',
    '#value' => $mapper->cfid,
  );
  $form['info'] = array(
    '#markup' => t('Delete filters for %title. This action can not be undone.', array('%title' => $mapper->title)),
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#name' => 'delete',
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#name' => 'cancel',
  );
  return $form;
}

/**
 * Submit function to delete filters for a mapper.
 */
function capx_filters_delete_submit($form, &$form_state) {
  if ($form_state['clicked_button']['#name'] == 'delete') {
    $mapper = reset(entity_load('capx_cfe', array($form_state['values']['id'])));
    unset($mapper->settings['filters']);
    entity_save('capx_cfe', $mapper);
    drupal_set_message(t('Filters deleted for %title', array('%title' => $mapper->title)));
  }
  $form_state['redirect'] = 'admin/config/capx/filters';
}