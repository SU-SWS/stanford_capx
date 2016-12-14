<?php
/**
 * @file
 * CAPx Tamper Theme functions.
 */

/**
 * Process sortable table.
 *
 * @param array $vars
 *   Variables.
 */
function theme_capx_tamper_list_form($vars) {
  $form = $vars['form'];

  /** @var \CAPx\Drupal\Entities\CFEntity $mapper */
  $mapper = capx_tamper_mapper_load($form['mapper']['#value']);
  $sources = capx_tamper_get_mapper_sources($mapper);
  foreach ($sources as &$targets) {
    foreach ($targets as &$field_name) {
      $instance = field_info_instance($mapper->entity_type, $field_name, $mapper->bundle_type);
      if ($instance) {
        $field_name = $instance['label'];
      }
    }
  }

  $header = array(
    t('Description'),
    t('Plugin'),
    t('Status'),
    t('Operations'),
    t('Weight'),
  );

  $output = drupal_render($form['goback']);

  foreach (element_children($form['tampers']) as $path) {
    $paths[] = $path;
    $rows = array();
    foreach (element_children($form['tampers'][$path]) as $tamper_id) {
      $rows[] = array(
        'data' => array(
          drupal_render($form['tampers'][$path][$tamper_id]['description']),
          drupal_render($form['tampers'][$path][$tamper_id]['plugin_id']),
          drupal_render($form['tampers'][$path][$tamper_id]['status']),
          drupal_render($form['tampers'][$path][$tamper_id]['operations']),
          drupal_render($form['tampers'][$path][$tamper_id]['weight']),
        ),
        'class' => array('draggable', 'group-' . capx_tamper_table_id($path)),
      );
    }

    // Empty plugins.
    if (!$rows) {
      $rows[] = array(
        array(
          'data' => t('No plugins defined.'),
          'colspan' => count($header),
          'class' => array('add-tamper'),
        ),
      );
    }

    // Add Plugin Link.
    $add_link =  l(t('Add Plugin'), CAPX_TAMPER . "/{$mapper->getMachineName()}/tamper/add/" . bin2hex($path));
    // Put action-link class on for fanciness sake.
    $add = '<ul class="feeds-tamper-add action-links"><li>' . $add_link . '</li></ul>';
    // "Add plugin" link is the last row.
    $rows[] = array(
      'data' => array(array('data' => $add, 'colspan' => 5)),
      'class' => array('feeds-tamper-add'),
    );

    $output .= theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => capx_tamper_table_id($path)),
      'caption' => t('!from -> !to', array(
        '!from' => $path,
        '!to' => implode(', ', $sources[$path]),
      )),
    ));
  }

  $output .= drupal_render_children($form);

  foreach ($sources as $path => $targets) {
    $table_id = capx_tamper_table_id($path);
    drupal_add_tabledrag($table_id, 'order', 'sibling', capx_tamper_table_id($path) . '-weight');
  }

  return $output;
}


