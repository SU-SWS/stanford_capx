<?php
/**
 * @file
 * stanford_cap_api_profiles_layout.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function stanford_cap_api_profiles_layout_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}
