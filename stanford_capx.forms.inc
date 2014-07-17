<?php
/**
 * @file
 * @author [author] <[email]>
 *
 * A collection of forms
 *
 */

use CAPx\Drupal\Util\CAPx;

/**
 * [stanford_capx_forms_connect_form description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_connect_form($form, &$form_state) {

  $username = decrypt(variable_get('stanford_capx_username', ''));
  $connection = CAPx::testConnection();

   $form['auth'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authorization'),
    '#collapsible' => TRUE,
    '#collapsed' => $connection->status,
  );
  $form['auth']['description'] = array(
    '#markup' => t('Please enter your authentication information for the CAP API.'),
  );

  $form['auth']['stanford_capx_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID:'),
    '#default_value' => $username,
    '#required' => TRUE,
  );

  $form['auth']['stanford_capx_password'] = array(
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

  $form['advanced']['stanford_capx_api_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint'),
    '#description' => t('CAP API endpoint URI, only useful when switching between development/production environment.'),
    '#default_value' => variable_get('stanford_capx_api_base_url', 'https://api.stanford.edu'),
    '#required' => TRUE,
  );

  $form['advanced']['stanford_capx_api_auth_uri'] = array(
    '#type' => 'textfield',
    '#title' => t('Authentication URI'),
    '#description' => t('CAP API authentication URI.'),
    '#default_value' => variable_get('stanford_capx_api_auth_uri', 'https://authz.stanford.edu/oauth/token'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save connection settings'),
  );

  return $form;
}

/**
 * [stanford_capx_forms_connect_form_validate description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_connect_form_validate($form, &$form_state) {

  $username = !empty($form_state['values']['stanford_capx_password']) ? $form_state['values']['stanford_capx_username'] : decrypt(variable_get('stanford_capx_username', ''));
  $password = !empty($form_state['values']['stanford_capx_password']) ? $form_state['values']['stanford_capx_pasword'] : decrypt(variable_get('stanford_capx_password', ''));
  $authpoint = $form_state['values']['stanford_capx_api_auth_uri'];
  $endpoint = $form_state['values']['stanford_capx_api_base_url'];

  // Test new user credentials!
  if (!empty($form_state['values']['stanford_capx_password'])) {

    $connection = CAPx::testConnection($username, $password, $authpoint);

    if (!$connection->status) {
      form_set_error('stanford_capx_username', t("Error. Can't connect to Stanford CAP API. Please check your username and password."));
      form_set_error('stanford_capx_password');
    }

  }

  // Test endpoint
  if (!isset($connection)) {
    $connection = CAPx::testConnection($username, $password, $authpoint);
  }

  // No connection token means no auth. Just end here.
  if (!isset($connection->token)) {
    return;
  }

  $endpointTest = CAPx::testConnectionToken($connection->token, $endpoint);

  if (!$endpointTest->value) {
    form_set_error('stanford_capx_api_base_url', t("Error, could not connect to endpoint"));
  }

}

/**
 * [stanford_capx_forms_connect_form_submit description]
 * @param  [type] $form       [description]
 * @param  [type] $form_state [description]
 * @return [type]             [description]
 */
function stanford_capx_forms_connect_form_submit($form, &$form_state) {

  drupal_set_message('Configuration options were saved.');

  // Username and Pass
  if (!empty($form_state['values']['stanford_capx_username'])
  && !empty($form_state['values']['stanford_capx_password'])) {

    $username = encrypt($form_state['values']['stanford_capx_username']);
    $password = encrypt($form_state['values']['stanford_capx_password']);
    variable_set('stanford_capx_username', $username);
    variable_set('stanford_capx_password', $password);

  }

  // Endpoints
  variable_set('stanford_capx_api_base_url', $form_state['values']['stanford_capx_api_base_url']);
  variable_set('stanford_capx_api_auth_uri', $form_state['values']['stanford_capx_api_auth_uri']);


}

