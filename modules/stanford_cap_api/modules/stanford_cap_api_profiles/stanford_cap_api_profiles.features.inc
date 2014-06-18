<?php
/**
 * @file
 * stanford_cap_api_profiles.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function stanford_cap_api_profiles_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_views_api().
 */
function stanford_cap_api_profiles_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}

/**
 * Implements hook_node_info().
 */
function stanford_cap_api_profiles_node_info() {
  $items = array(
    'cap_stanford_profile' => array(
      'name' => t('Stanford profile'),
      'base' => 'node_content',
      'description' => '',
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}
