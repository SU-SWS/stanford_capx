<?php

/**
 * @file
 * @author [author] <[email]>
 *
 * A place to put your documentation related functionality.
 */

/**
 * [stanford_capx_admin_config description]
 * @return [type] [description]
 */
function stanford_capx_admin_config_data_browser() {
  // @TODO: Add refresh functionality?

  $base = drupal_get_path('module', 'stanford_capx');

  // Provide a place to render the tree.
  $output['content']['#markup'] = '<div id="capx-schema"></div>';

  // Load the CAP profile schema json.
  $schema = stanford_capx_schema_load('json');

  // Convert the schema to a jqTree compatible format.
  $jq_tree = stanford_capx_jqTree_get($schema);
  // @TODO: Cache this?

  // Attach the default jsTree CSS.
  $output['#attached']['css'] = array($base . '/js/jqTree/jqtree.css');

  // Attach libs, js & render the formatted array to Drupal.settings.
  $output['#attached']['js'] = array(
    $base . '/js/jqTree/tree.jquery.js',
    $base . '/js/stanford_capx.docs.js',
    array(
      'data' => array(
        'stanford_capx' => array(
          'schema' => $jq_tree,
        ),
      ),
      'type' => 'setting',
    ),
  );

  return $output;
}

/**
 * [stanford_capx_jqTree_get]
 * @param [type] [description]
 *
 * @return [type] [description]
 */
function stanford_capx_jqTree_get($schema_json) {
  $decoded_json = json_decode($schema_json);
  return stanford_capx_jqTree_render($decoded_json->properties);
}

/**
 * [stanford_capx_jqTree_render]
 * @param [type] [description]
 *
 * @return [type] [description]
 */
function stanford_capx_jqTree_render(&$schema) {
  $branch = array();
  $metadata = array();
  foreach ($schema as $name => $element) {
    $leaf = new stdClass();

    switch (gettype($element)) {
      case 'string':
        // Strings describe the properties of the element.
        switch ($name) {
          case 'title':
            $leaf->label = t('Title:') . ' ' . check_plain($element);
            $metadata[0] = $leaf;
            break;

          case 'type':
            $leaf->label = t('Type:') . ' ' . check_plain($element);
            $metadata[1] = $leaf;
            break;

          case 'description':
            $leaf->label = t('Description:') . ' ' . check_plain($element);
            $metadata[2] = $leaf;
            break;
        }
        break;

      case 'object':
        // An object indicates another child (branch) in the tree to render.
        $new_branch = new stdClass();
        $new_branch->label = check_plain($name);

        if ($element->type === 'object') {
          $new_branch->children = stanford_capx_jqTree_render($element->properties);
        }
        else {
          $new_branch->children = stanford_capx_jqTree_render($element);
        }

        $branch[] = $new_branch;
        break;
    }
  }

  return array_merge($metadata, $branch);
}
