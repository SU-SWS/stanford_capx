<?php
/**
 * @file
 * stanford_cap_api_profiles.features.filter.inc
 */

/**
 * Implements hook_filter_default_formats().
 */
function stanford_cap_api_profiles_filter_default_formats() {
  $formats = array();

  // Exported format: Full HTML for CAP profiles.
  $formats['full_html_for_cap_profiles'] = array(
    'format' => 'full_html_for_cap_profiles',
    'name' => 'Full HTML for CAP profiles',
    'cache' => 1,
    'status' => 1,
    'weight' => 0,
    'filters' => array(),
  );

  return $formats;
}
