<?php
/**
 * @file
 * @author [author] <[email]>
 *
 * A collection of forms
 *
 */


/**
 * [stanford_capx_forms_connect_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_connect_form($form, &$form_state) {
  $form['auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authorization'),
  );

  $form['auth']['description_wrapper'] = array(
    '#type' => 'container',
  );
  $form['auth']['description_wrapper']['description'] = array(
    '#markup' => t('Please enter your authentication information for the CAP API.'),
  );

  $form['auth']['stanford_cap_api_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID:'),
    '#default_value' => variable_get('stanford_cap_api_username', ''),
    '#required' => TRUE,
  );

  $form['auth']['stanford_cap_api_password'] = array(
    '#type' => 'password',
    '#title' => t('Authz secret:'),
  );

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Advanced setting for CAP API and authentication URIs'),
  );

  $form['advanced']['stanford_cap_api_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint'),
    '#description' => t('CAP API endpoint URI, only useful when switching between development/production environment.'),
    '#default_value' => variable_get('stanford_cap_api_base_url', 'https://api.stanford.edu'),
    '#required' => TRUE,
  );

  $form['advanced']['stanford_cap_api_auth_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Authentication URI'),
    '#description' => t('CAP API authentication URI.'),
    '#default_value' => variable_get('stanford_cap_api_auth_uri', 'https://authz.stanford.edu/oauth/token'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save connection settings'),
  );

  return $form;
}
