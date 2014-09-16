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
 * @param [type] [description]
 *
 * @return [type] [description]
 */
function stanford_capx_jqTree_render(&$schema, $parents = array()) {
  // Initialize variables for collecting the field metadata.
  $branch = array();
  $metadata = array();

  // Iterate over each property of the schema objects.
  foreach ($schema as $name => $element) {
    // Initialize the leaf to collect a single piece of metadata E.g. Type.
    $leaf = new stdClass();

    // Is the current object property a string or a child object?
    switch (gettype($element)) {
      case 'string':
        // Strings describe the field metadata.
        switch ($name) {
          case 'type':
            // Render field type.
            $leaf->label = t('Type:') . ' ' . check_plain($element);
            $metadata[1] = $leaf;

            // Calculate and render the JSON selector.
            $selector = new stdClass();
            $selector->label = t('Selector:') . ' $.' . implode($parents, '.');
            $metadata[4] = $selector;
            break;

          case 'description':
            // Render field description.
            $leaf->label = t('Description:') . ' ' . check_plain($element);
            $metadata[2] = $leaf;
            break;

          case 'metadata':
            // Render field metadata property. Always 'true' if present.
            $leaf->label = t('Metadata:') . ' ' . check_plain($element);
            $metadata[3] = $leaf;
            break;
        }
        break;

      case 'object':
        // An object indicates another child (branch) in the tree to render.
        $child_branch = new stdClass();

        // Fall back to the element name if title property not set.
        $child_branch->label = isset($element->title) ? check_plain($element->title) : check_plain($name);

        // Push the current element name for calculating the JSON selector.
        array_push($parents, $name);

        // Recursively process child properties - avoid rendering "properties".
        if (isset($element->properties)) {
          $child_branch->children = stanford_capx_jqTree_render($element->properties, $parents);
        }
        else {
          $child_branch->children = stanford_capx_jqTree_render($element, $parents);
        }

        // Pop the current element name; it was just recursively processed.
        array_pop($parents);

        // Add the child branch to the current branch.
        $branch[] = $child_branch;
        break;
    }
  }

  // Sort the display order of element metadata by key.
  ksort($metadata);

  // Merge the arrays, forcing the metadata to the top of each property.
  return array_merge($metadata, $branch);
}
