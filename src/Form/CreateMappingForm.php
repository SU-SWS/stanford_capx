<?php

namespace Drupal\stanford_capx\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Filter;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Utility\Html;
use SUSWS\APIAuthLib\Auth;
use GuzzleHttp\Client;
use GuzzleHttp\Exception\ConnectException;
use GuzzleHttp\Exception\ClientException;

/**
 * Implements the CAPx CreateMappingForm.
 */
class CreateMappingForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'stanford_capx_create_mapping';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $mapping = NULL;

    // Get the config.
    //$config = \Drupal::config('stanford_capx.settings');

    $form = array();
    $form += $this->getMachineNameForm($form, $form_state, $mapping);
    $form += $this->getEntityMappingForm($form, $form_state, $mapping);
    $form += $this->getFieldMappingForm($form, $form_state, $mapping);
    /*
     $form['mapping']['description'] = array(
       '#markup' => t("After you have connected to CAP, create a Mapping to link CAP&#8217;s fields with your fields"),
     );

     $form['variables']['batch_limit'] = array(
       '#title' => $this->t('Batch, or cron, processing limit'),
       '#type' => 'number',
       '#default_value' => $config->get('batch_limit'),
       '#description' => t('This is the number of items to process in one sync operation.'),
       '#min' => 1,
       '#max' => 1000,
       '#required' => TRUE,
       '#size' => 10,
       '#step' => 1,
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
   */

    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => $this->t('Save mapping'),
      '#button_type' => 'primary',
    );

    $link_options = array('attributes' => array('class' => array('btn button')), 'query' => array('destination' => Url::fromRoute('<current>')->toString()));

    if (isset($mapping)) {
      $delete_url = URL::fromUri("internal:/admin/config/capx/mapping/delete");
      $delete_url->setOptions($link_options);
      $delete_link = Link::fromTextAndUrl(t("Delete Mapping"), $delete_url)
                         ->toString();
      $form['actions']['delete'] = array(
        '#markup' => "<p>" . $delete_link . "</p>",
      );
    }

    $cancel_url = URL::fromUri("internal:/admin/config/capx/mapping");
    $cancel_link = Link::fromTextAndUrl(t("Cancel"), $cancel_url)
                       ->toString();
    $form['actions']['cancel'] = array(
      '#markup' => "<p>" . $cancel_link . "</p>",
    );

    return $form;
  }


  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    /*
      // Get the config.
      $config = \Drupal::service('config.factory')
        ->getEditable("stanford_capx.settings");

      // Get new data.
      $batch_limit = Html::escape($form_state->getValue('batch_limit'));
      $default_field_format = Html::escape($form_state->getValue('default_field_format'));

      // Set the new data
      $config->set('batch_limit', $batch_limit)
        ->set("default_field_format", $default_field_format)
        ->save();

    */
    $this->messenger()->addMessage("CAPx mapping updated successfully.");
  }

  /**
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   * @param null $mapping
   * @return array
   */
  private function getMachineNameForm (array $form, FormStateInterface $form_state, $mapping = NULL) {
    $form['naming'] = array(
      '#title' => t('Mapping title'),
      '#type' => 'fieldset',
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
      '#description' => t('Provide a human-readable name and machine name for this set of mapping configuration.'),
    );

    $form['naming']['title'] = array(
      '#type' => 'textfield',
      '#description' => isset($mapping->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t('Please enter a unique name for this mapper'),
      '#default_value' => isset($mapping->title) ? $mapping->title : '',
      '#disabled' => isset($mapping->machine_name),
    );

    $form['naming']['machine-name'] = array(
      '#type' => 'machine_name',
      '#title' => t('Machine name'),
      '#default_value' => isset($mapping->machine_name) ? $mapping->machine_name : '',
      '#size' => 64,
      '#maxlength' => 64,
      '#description' => isset($mapping->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t('A unique name for the mapping. It must only contain lowercase letters, numbers and hyphens.'),
      '#machine_name' => array(
        'exists' => 'stanford_capx_mapper_machine_name_exits',
        'source' => array('naming','title'),
      ),
      '#disabled' => isset($mapping->machine_name),
    );

    return $form;

  }

  /**
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   * @param null $mapping
   * @return array
   */
  private function getEntityMappingForm (array $form, FormStateInterface $form_state, $mapping = NULL) {

    /*
    $entity_types = capx_entity_get_info();
    $entity_options = array();
    $bundle_options = array();

    foreach ($entity_types as $entity_name => $values) {
      $entity_options[$entity_name] = $values['label'];
      $bundle_options[$entity_name] = array();

      foreach($values['bundles'] as $bundle_machine_name => $bundle_info) {
        $bundle_options[$entity_name][$bundle_machine_name] = $bundle_info['label'];
      }

    }

    */
    $form['entity-mapping'] = array(
      '#type' => 'fieldset',
      '#title' => t('Entity Mapping'),
      '#description' => t('Select which entity type and bundle you would like to map CAP data into. Please select the entity type first and the bundle type will appear. The entity and bundle has to exist before creating a mapping to it.'),
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
    );

    /*
    $form['entity-mapping']['entity-type'] = array(
      '#type' => 'select',
      '#title' => t("Select entity type"),
      '#description' => isset($mapper->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t(''),
      '#options' => $entity_options,
      '#default_value' => isset($mapper->entity_type) ? $mapper->entity_type : 'node',
      '#disabled' => isset($mapper->machine_name),
    );

    // Build out the bundles options.
    foreach ($bundle_options as $entity_name => $bundle_opts) {

      $form['entity-mapping']['bundle-'.$entity_name] = array(
        '#type' => 'select',
        '#title' => t('Select bundle'),
        '#description' => isset($mapper->machine_name) ? t("This field has been disabled and cannot change once it has been set.") : t(''),
        '#options' => $bundle_opts,
        '#default_value' => isset($mapper->bundle_type) ? $mapper->bundle_type : 'stanford_person',
        '#states' => array(
          'visible' => array('select[name="entity-type"]' => array('value' => $entity_name)),
        ),
        '#disabled' => isset($mapper->machine_name),
      );

    }

    $form['entity-mapping']['multiple-entities'] = array(
      '#type' => 'checkbox',
      '#title' => t("Would you like to create multiple entities per imported bundle for the type of content you're importing?"),
      '#description' => t('By default, a mapper will create one entity per person. If you wanted to import a persons publications you would need multiple entities per person. Check this box for multiple.'),
      '#default_value' => isset($mapper->multiple) ? $mapper->multiple : FALSE,
    );

    $form['entity-mapping']['subquery'] = array(
      '#type' => 'textfield',
      '#title' => t("Multiple entity creation sub query"),
      '#description' => t('Which part of the schema will you be using to create multiple entities. Ie: for publications enter: $.publications.*'),
      '#default_value' => isset($mapper->subquery) ? $mapper->subquery : '',
      '#states' => array(
        'visible' => array(
          ':input[name="multiple-entities"]' => array('checked' => TRUE),
        ),
      ),
    );

    $form['entity-mapping']['guuid-query'] = array(
      '#type' => 'textfield',
      '#title' => t("Unique ID of entity"),
      '#description' => t('Do the entities you are creating have a unique id? For example, publications have a publicationId and $.publications.*.publicationId would be the value to add to this field. If this is provided then CAPx can update the entity in place. If there is no unique ID available then the CAPx module will delete and re-import the entities each time there is a change to the profile.'),
      '#default_value' => isset($mapper->guuidquery) ? $mapper->guuidquery : '',
      '#states' => array(
        'visible' => array(
          ':input[name="multiple-entities"]' => array('checked' => TRUE),
        ),
      ),
    );
    */

    return $form;
  }

  /**
   * @param array $form
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   * @param null $mapping
   */

  private function getFieldMappingForm (array $form, FormStateInterface $form_state, $mapping = NULL) {

    $form['field-mapping'] = array(
      '#type' => 'fieldset',
      '#title' => t('Field Mapping'),
      '#collapsed' => FALSE,
      '#collapsible' => FALSE,
    );
    return $form;
  }
}
