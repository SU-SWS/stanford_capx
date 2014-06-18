<?php

/**
 * @file
 * Theme functions.
 */


function theme_stanford_cap_api_profiles_field_tree($variables) {

  _stanford_cap_api_profiles_field_tree($variables['schema_tree'], $items);
  $vars = array(
    'type' => 'ul',
    'items' => $items,
    'attributes' => array('id' => 'fields'),
  );

  return theme('item_list', $vars);
}

/**
 * Renders fields checkboxes into tree of lists.
 */
function _stanford_cap_api_profiles_field_tree(&$elem, &$items = array()) {
  foreach (element_children($elem) as $key) {
    $class = $elem[$key]['#attributes']['class'];
    unset($elem[$key]['#attributes']['class']);
    $items[] = array(
      'class' => $class,
      'data' => drupal_render($elem[$key]),
    );
    unset($elem[$key]);
  }
}
