<?php
/**
 * @file
 * stanford_cap_api_profiles.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function stanford_cap_api_profiles_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_anonymous_cap_stanford_profile';
  $strongarm->value = 0;
  $export['comment_anonymous_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_cap_stanford_profile';
  $strongarm->value = '1';
  $export['comment_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_mode_cap_stanford_profile';
  $strongarm->value = 1;
  $export['comment_default_mode_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_default_per_page_cap_stanford_profile';
  $strongarm->value = '50';
  $export['comment_default_per_page_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_form_location_cap_stanford_profile';
  $strongarm->value = 1;
  $export['comment_form_location_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_preview_cap_stanford_profile';
  $strongarm->value = '1';
  $export['comment_preview_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'comment_subject_field_cap_stanford_profile';
  $strongarm->value = 1;
  $export['comment_subject_field_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_options_cap_stanford_profile';
  $strongarm->value = array(
    0 => 'main-menu',
  );
  $export['menu_options_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'menu_parent_cap_stanford_profile';
  $strongarm->value = 'main-menu:0';
  $export['menu_parent_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_options_cap_stanford_profile';
  $strongarm->value = array(
    0 => 'status',
  );
  $export['node_options_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_preview_cap_stanford_profile';
  $strongarm->value = '1';
  $export['node_preview_cap_stanford_profile'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'node_submitted_cap_stanford_profile';
  $strongarm->value = 0;
  $export['node_submitted_cap_stanford_profile'] = $strongarm;

  return $export;
}
