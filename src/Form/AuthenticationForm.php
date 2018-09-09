<?php

namespace Drupal\stanford_capx\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use SUSWS\APIAuthLib\Auth;
use GuzzleHttp\Client;
use GuzzleHttp\Exception\ConnectException;
use GuzzleHttp\Exception\ClientException;

/**
 * Implements the CAPx Authentication form.
 */
class AuthenticationForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'stanford_capx_authentication_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    // Get the config.
    $config = \Drupal::config('stanford_capx.settings');

    // Credentials.
    $form['credentials'] = array(
      '#type' => 'fieldset',
      '#title' => $this->t('Credentials'),
    );

    $url = URL::fromUri("https://helpsu.stanford.edu/helpsu/3.0/auth/helpsu-form?pcat=CAP_API&dtemplate=CAP-OAuth-Info");
    $link = link::fromtextandurl(t("File a HelpSU request"), $url)->toString();
    $form['credentials']['description'] = array(
      '#markup' => t('Please enter your authentication information for the CAP API. If you don\'t have these credentials yet you can @helpsu.',
        array('@helpsu' => $link)),
    );

    $form['credentials']['username'] = array(
      '#type' => 'textfield',
      '#title' => $this->t('Username'),
      '#default_value' => $config->get("username"),
      '#size' => 60,
      '#required' => TRUE,
    );

    $form['credentials']['password'] = array(
      '#type' => 'password',
      '#title' => $this->t('Password'),
      '#size' => 25,
      '#required' => TRUE,
    );

    // Endpoints.
    $form['endpoints'] = array(
      '#type' => 'fieldset',
      '#title' => $this->t('Endpoints'),
    );

    $form['endpoints']['description'] = array(
      '#markup' => t('Endpoint settings for authentication URIs and CAP API'),
    );

    $form['endpoints']['oauth'] = array(
      '#type' => 'url',
      '#title' => $this->t('OAuth Server'),
      '#default_value' => $config->get("oauth"),
      '#description' => t('CAP API authentication URI.'),
      '#size' => 60,
      '#required' => TRUE,
    );

    $form['endpoints']['api'] = array(
      '#type' => 'url',
      '#title' => $this->t('API Server'),
      '#default_value' => $config->get("api"),
      '#description' => t('CAP API endpoint URI, only useful when switching between development/production environment.'),
      '#size' => 60,
      '#required' => TRUE,
    );

    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => $this->t('Authenticate'),
      '#button_type' => 'primary',
    );

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    // Test to see if we can authenticate.
    $username = $form_state->getValue('username');
    $password = $form_state->getValue('password');
    $authpoint = $form_state->getValue('oauth');

    // See if we can fetch a token.
    $guzzle = new Client(['defaults' => ['auth' => 'oauth']]);
    $auth = new Auth($guzzle, $authpoint);
    try {
      $auth->authenticate($username, $password);
    }
    catch (ConnectException $e) {
      $form_state->setErrorByName('oauth', $this->t('Could not reach oauth server.'));
      return;
    }
    catch (ClientException $e) {
      $form_state->setErrorByName('username', $this->t('Invalid credentials.'));
      $form_state->setErrorByName('password', $this->t('Invalid credentials.'));
      return;
    }

    if (!$auth->getAuthApiToken()) {
      $form_state->setErrorByName('username', $this->t('Failed to authenticate.'));
      $form_state->setErrorByName('password', $this->t('Failed to authenticate.'));
    }

  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // Get the config.
    $config = \Drupal::service('config.factory')
      ->getEditable("stanford_capx.settings");

    // Save new data.
    $username = $form_state->getValue('username');
    $password = $form_state->getValue('password');
    $authpoint = $form_state->getValue('oauth');
    $endpoint = $form_state->getValue('api');

    $config->set("username", $username)
      ->set("password", $password)
      ->set("oauth", $authpoint)
      ->set("api", $endpoint)
      ->save();

    $this->messenger()->addMessage("Connected to the API successfully.");
  }

}
