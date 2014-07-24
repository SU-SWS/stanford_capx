<?php
/**
 * @file
 * Theme functions.
 */

/**
 * Implements hook_theme().
 */
function stanford_capx_theme($existing, $type, $theme, $path) {
  return array(
    'stanford_capx_columns_to_the_right' => array(
      'render element' => '',
    ),
  );
}

/**
 * Themes a form container by putting some information to the left and
 * all of the fields to the right in a consistent table. Uses global striping.
 * @return [type] [description]
 */
function theme_stanford_capx_columns_to_the_right($elements) {
  $element = array_pop($elements);
  global $stanford_capx_theme_zebra;
  $stanford_capx_theme_zebra = ($stanford_capx_theme_zebra == "even") ? "odd" : "even";
  $header = array("<b>" . t($element['#title']) . "</b>", t('CAP API Path'));

  $info = "";
  if (isset($element['#field_info']['field_name'])) {
    $info .= "<p><b>Machine Name</b>: <em>" . $element['#field_info']['field_name'] . "</em></p>";
  }
  $info .= "<p><b>Description</b>: " . t($element['#description']) . "</p>";

  $rows = array();
  $rows[] = array(
    'data' => array(
      array('data' => $info, 'class' => 'info-cell', 'style' => 'width: 60%;'),
      array('data' => $element['#children']),
    ),
    'class' => array($stanford_capx_theme_zebra),
    'no_striping' => TRUE,
  );

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'class' => 'capx_mapper_table',
  ));

  return $output;
}
