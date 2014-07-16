<?php
/**
 * @file
 * @author [author] <[email]>
 */

namespace CAPx\Drupal\Processors\FieldProcessors;
use CAPx\Drupal\Processors\FieldProcessors\FieldProcessorAbstract;

/**
 * List of known fields and widgets
 *
 *Type name                Default widget              Widgets
 * datetime                 date_select
 *
 * date                     date_select
 *
 * datestamp                date_select
 *
 * email                    email_textfield
 * field_collection         field_collection_hidden
 *
 * file                     file_generic
 * image                    image_image
 * link_field               link_field
 * list_integer             options_select
 *
 * list_float               options_select
 *
 * list_text                options_select
 *
 * list_boolean             options_buttons
 *
 * number_integer           number
 * number_decimal           number
 * number_float             number
 * taxonomy_term_reference  options_select
 *
 *
 * text                     text_textfield
 * text_long                text_textarea
 * text_with_summary        text_textarea_with_summary
 *
 */


class FieldProcessor extends FieldProcessorAbstract {

  /**
   * [field description]
   * @param  [type] $type [description]
   * @return [type]       [description]
   */
  public function field($type) {

    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();

    switch ($type) {

      case "date":
        $processor = new DateFieldProcessor($entity, $fieldName, $type);
        break;

      case "datetime":
        $processor = new DatetimeFieldProcessor($entity, $fieldName, $type);
        break;

      case "datestamp":
        $processor = new DatestampFieldProcessor($entity, $fieldName, $type);
        break;

      case "email":
        $processor = new EmailFieldProcessor($entity, $fieldName);
        break;

      case "field_collection":
        $processor = new FieldCollectionProcessor($entity, $fieldName);
        break;

      case "file":
        $processor = new FileFieldProcessor($entity, $fieldName, $type);
        break;

      case "image":
        $processor = new ImageFieldProcessor($entity, $fieldName, $type);
        break;

      case "link_field":
        $processor = new LinkFieldProcessor($entity, $fieldName);
        break;

      case "list_integer":
      case "list_float":
      case "list_text":
      case "list_boolean":
        $processor = new ListFieldProcessor($entity, $fieldName, $type);
        break;

      case "number_integer":
      case "number_decimal":
      case "number_float":
        $processor = new NumberFieldProcessor($entity, $fieldName, $type);
        break;

      case "taxonomy_term_reference":
        $processor = new TaxonomyTermFieldProcessor($entity, $fieldName);
        break;

      case "text":
        $processor = new TextFieldFieldProcessor($entity, $fieldName);
        break;

      case "text_long":
      case "text_with_summary":
        $processor = new TextAreaFieldProcessor($entity, $fieldName, $type);
        break;

      default:
        $processor = $this;
    }

    drupal_alter('capx_field_processor_field', $processor, $type, $fieldName, $entity);

    return $processor;

  }

  /**
   * Widget processing.
   * @param  [type] $type [description]
   * @return [type]              [description]
   */
  public function widget($type) {

    $entity = $this->getEntity();
    $fieldName = $this->getFieldName();

    switch ($type) {

      // Downloads and saves a file
      case "image_file":
      case "image_image":
        $processor = new ImageFieldWidgetProcessor($entity, $fieldName);
        break;

      default:
        $processor = $this;
    }

    drupal_alter("capx_field_processor_widget", $processor, $type, $fieldName, $entity);

    return $processor;
  }

}
