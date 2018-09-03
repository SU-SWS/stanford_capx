<?php

namespace Drupal\stanford_capx\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Filter;
use Drupal\Core\Link;
use Drupal\Core\Url;
use SUSWS\APIAuthLib\Auth;
use GuzzleHttp\Client;
use GuzzleHttp\Exception\ConnectException;
use GuzzleHttp\Exception\ClientException;

/**
 * Implements the CAPx Settingsform.
 */
class SettingsForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'stanford_capx_settings_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

    // Get the config.
    $config = \Drupal::config('stanford_capx.settings');

    // Variables.
    $form['variables'] = array(
      '#title' => $this->t('Synchronization settings'),
      '#type' => 'fieldset',
    );

    $form['variables']['description'] = array(
      '#markup' => t('These are the settings for update and synchronization actions with the CAP API.'),
    );

    $form['variables']['batch_limit'] = array(
      '#title' => $this->t('Batch, or cron, processing limit'),
      '#type' => 'textfield',
      '#default_value' => $config->get("batch_limit"),
      '#description' => t('This is the number of items to process in one sync operation.'),
      '#size' => 5,
      '#required' => TRUE,
    );

    $options = array();
    $formats = filter_formats();
    foreach ($formats as $format) {
      $options[$format->id()] = $format->label();
    }

    $form['variables']['default_field_format'] = array(
      '#title' => $this->t('Text format'),
      '#type' => 'select',
      '#required' => TRUE,
      '#options' => $options,
      '#default_value' => $config->get("default_field_format"),
      '#description' => t('This text format will be applied to any fields that have text processing enabled.'),
    );

    // Organizations and schemas.
    $form['orggroup'] = array(
      '#type' => "fieldset",
      '#title' => t("Organizations & Schema"),
      '#description' => t("The CAPx module need information from the CAP API in order to function properly. Below are actions that require communication with the CAP API server and you must be connected before you can run these tasks."),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    // Organizations.
    $form['orggroup']['orgcodeinfo']['#markup'] = "<h3>Organizations</h3>";
    $form['orggroup']['orgcodeinfo']['#markup'] .= "<p>" . t("Organization codes come from the CAP API and are saved to a taxonomy. This taxonomy powers the organization code autocomplete in the import section.") . "</p>";
    $form['orggroup']['orgcodeinfo']['#markup'] .= "<p>" . t("Last sync: @date", array("@date" => $config->get("last_orgs_sync"))) . "</p>";

    $url = URL::fromUri("internal:/admin/structure/taxonomy");
    $link = Link::fromTextAndUrl(t("Organization taxonomy"), $url)->toString();
    $form['orggroup']['orgcodeinfo']['#markup'] .= "<p>" . t("View all organization codes: @link", array("@link" => $link)) . "</p>";

    $url = URL::fromUri("internal:/admin/config/capx/organizations/sync");
    $link_options = array('attributes' => array('class' => array('btn button')), 'query' => array('destination' => Url::fromRoute('<current>')->toString()));
    $url->setOptions($link_options);
    $link = Link::fromTextAndUrl(t("Get organization data"), $url)->toString();
    $form['orggroup']['orgcodeinfo']['#markup'] .= "<p>" . $link . "</p>";

    // Schema.
    $form['orggroup']['schemainfo']['#markup'] = "<br /><h3>Schema Information</h3>";
    $form['orggroup']['schemainfo']['#markup'] .= "<p>" . t("The CAP API provides a schema of the information available. The CAPx module uses schema information to populate data browsers on the mapping pages.") . "</p>";
    $form['orggroup']['schemainfo']['#markup'] .= "<p>" . t("Last sync: @date", array("@date" => $config->get("last_schema_sync"))) . "</p>";
    $form['orggroup']['submit'] = array(
      '#type' => 'button',
      '#value' => 'Get schema information',
      '#op' => 'submit',
    );

    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => $this->t('Save settings'),
      '#button_type' => 'primary',
    );

    return $form;
  }


  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    /*
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

    */
    }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    /*
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

    */
      $this->messenger()->addMessage("CAPx settings updated successfully.");
    }

}