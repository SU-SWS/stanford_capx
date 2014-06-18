<?php
/**
 *  This file contains a routine the defines for most CAP fields a corresponding
 *  Drupal field type and a callback routine to return the CAP value for that Drupal field.
 *  
 *  The routine invokes hook_alter so that other modules can add or replace field definitions.
 *  For example, a module might add the clinician related fields, or might replace the callbacks
 *  with ones to build field collections.
 *
 *  Finally, the callback routines are also defined here to work with the CAP API v1.
 *
 *  Fields matching the specified field_type must be defined in the Drupal UI for whichever 
 *  entity (content type or user account) is being used to receive CAP data. CAP fields are
 *  matched against the specific Drupal fields in the Configuration|People|CAP Lite forms.
 */

function _ses_cap_lite_profile_fields() {
    $cap_fields = array(
        'administrativeAppointments' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'alias' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_cap_link'),
        'alternateContact' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_contact_fields'),
        'alternateContactLabel' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'alternateContactName' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'alternateContactTitle' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'alternateContactPhone' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'alternateContactEmail' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'), 
        'bio' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_html_field'),
        'communityAndInternationalWork' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'coursesTaught' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_courses_list'),
        'currentResearchInterestsFull' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_research_full'),
        'currentResearchInterestsShort' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_research_short'),
        'currentResearchInterestsTerse' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_research_terse'),
        'currentRoleAtStanford' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_string'),
        'displayName' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_string'),
        'education' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'graduateAndFellowshipPrograms' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'honorsAndAwards' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'internetLinks' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'keywords' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_keywords'),
        'longTitle' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_title'),
        'patents' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'personalInterests' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_html_field'),
        'postdoctoralAdvisees' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'presentations' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'primaryContact' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_contact_fields'),
        'primaryContactName' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'primaryContactTitle' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'primaryContactPhone' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'primaryContactEmail' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_contact_field'),
        'profileId' => array(
            'field_type' => 'number_integer',
            'build_callback' => '_ses_cap_lite_get_profile_id'), 
        'professionalInterests' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_html_field'),
        'professionalOrganizations' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'profilePhotos' => array(
            'field_type' => 'image',
            'build_callback' => '_ses_cap_lite_get_profile_photo'),
        'projects' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'publications' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_publications_list'),
        'serviceWork' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'shortTitle' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_title'),
        'stanfordAdvisors' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'titles' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
        'uid' => array(
            'field_type' => 'text',
            'build_callback' => '_ses_cap_lite_get_string'),
        'workExperience' => array(
            'field_type' => 'text_long',
            'build_callback' => '_ses_cap_lite_get_field_list'),
    );
    drupal_alter('ses_cap_lite_field_array',$cap_fields);
    ksort($cap_fields);
    return $cap_fields;
}

/*
 * This is a generic routine used to format CAP fields containing 0 to n elements,
 * each entered separately in the CAP system. Examples are education and workExperience
 *
 * The field is constructed using <ul> and <li> tags
 */
function _ses_cap_lite_get_field_list($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem) || !is_array($capDataItem)) {
        $output = array();
    } else {
        $output = '<div class="'.$cap_field.'-div"><ul class="'.$cap_field.'-ul">';
        foreach($capDataItem as $lineitem) {
            $output .= '<li class="'.$cap_field.'-li">'.$lineitem['label']['html'].'</li>';
        }
        $output .= '</ul></div>';
        $output = array(LANGUAGE_NONE=>array(array('value'=>$output,
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/* 
 * this function is used to return either the CAP longTitle or CAP shortTitle
 *
 */
function _ses_cap_lite_get_title($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem) || !is_array($capDataItem)) {
        $output = array();
    } else {
        $output = array(LANGUAGE_NONE=>array(array('value'=>$capDataItem['label']['text'])));
    }
    return $output;
}

/*
 * This functon is used to return the fullText field of the CAP currentResearchInterests field
 *
 */
function _ses_cap_lite_get_research_full($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData['currentResearchInterests'])) return array();
    $capDataItem = $capData['currentResearchInterests'];
    if (empty($capDataItem) || !is_array($capDataItem) || empty($capDataItem['fullText']['html'])) {
        $output = array();
    } else {
        $output = array(LANGUAGE_NONE=>array(array('value'=>'<div class="'.$cap_field.'-div">'.$capDataItem['fullText']['html'].'</div>',
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/*
 * This function is used to return the shortText field of the CAP currentResearchInterests field
 *
 */
function _ses_cap_lite_get_research_short($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData['currentResearchInterests'])) return array();
    $capDataItem = $capData['currentResearchInterests'];
    if (empty($capDataItem) || !is_array($capDataItem) || empty($capDataItem['shortText']['text'])) {
        $output = array();
    } else {
        $output = array(LANGUAGE_NONE=>array(array('value'=>$capDataItem['shortText']['text'])));
//        $output = array(LANGUAGE_NONE=>array(array('value'=>'<div class="'.$cap_field.'-div">'.$capDataItem['shortText']['html'].'</div>',
//            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/*
 * This function is used to return the terseText field of the CAP currentResearchInterests field
 * 
 */
function _ses_cap_lite_get_research_terse($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData['currentResearchInterests'])) return array();
    $capDataItem = $capData['currentResearchInterests'];
    if (empty($capDataItem) || !is_array($capDataItem) || empty($capDataItem['terseText']['text'])) {
        $output = array();
    } else {
        $output = array(LANGUAGE_NONE=>array(array('value'=>$capDataItem['terseText']['text'])));
//        $output = array(LANGUAGE_NONE=>array(array('value'=>'<div class="'.$cap_field.'-div">'.$capDataItem['terseText']['html'].'</div>',
//            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }   
    return $output;
} 

function _ses_cap_lite_get_profile_id($capData = array(), $cap_field) {
    if (!isset($capData['profileId'])) return array();
    $output = array(LANGUAGE_NONE=>array(array('value'=>$capData['profileId'])));
    return $output; 
}

function _ses_cap_lite_get_html_field($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem) || !is_array($capDataItem)) {
        $output = array();
    } else {
        $output = array(LANGUAGE_NONE=>array(array('value'=>'<div class="'.$cap_field.'-div">'.$capDataItem['html'].'</div>',
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/* 
 * This function is used to return the contents of either the primaryContact or alternateContact
 * CAP fields. It returns all the field components together.
 *
 */
function _ses_cap_lite_get_contact_fields($capData =  array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    $output = array();
    if (!empty($capDataItem) && is_array($capDataItem)) {
        $output = '';
        if ($cap_field === 'alternateContact' && !empty($capDataItem['label'])) {
            $output .= '<div class="cap-contact">'.$capDataItem['label'];
        } else {
            $output .= '<div class="cap-contact">Contact';
        }
        if (!empty($capDataItem['name'])) {
            $ouput .= '<div class="cap-contact-name">'.$capDataItem['name'].'</div>';
        }
        if (!empty($capDataItem['title'])) {
            $output .= '<div class="cap-contact-title">'.$capDataItem['title'].'</div>';
        }
        if (!empty($capDataItem['phoneNumbers']) && is_array($capDataItem['phoneNumbers'])) {
            $output .= '<div class="cap-contact-phones">';
            $output .= '<div class="cap-contact-phones-label">Phone:</div>';
            foreach ($capDataItem['phoneNumbers'] as $capPhone) {
                $output .= '<div class="cap-contact-phone-number">'.$capPhone.'</div>';
            }
            $output .= '</div>';
        }
        if (!empty($capDataItem['email'])) {
            $output .= '<div class="cap-contact-email">';
            $output .= '<div class="cap-contact-email-label">Email:</div>';
            $output .= '<div class="cap-contact-email-address">'.$capDataItem['email'].'</div>';
            $output .= '</div>';
        }
        $output .= '</div>';
    }
    if (empty($output)) {
        $output = array();
    } else {
        $output = array(LANGUAGE_NONE=>array(array('value'=>$output,
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/* 
 * This function is used to return an individual component of a CAP contact group (either
 * primary contact or alternate contact) as a single text field.
 */
function _ses_cap_lite_get_contact_field($capData = array(),$cap_field) {
    $realCapField = NULL;
    if (strpos($cap_field,'alternate') !== FALSE) {
        $realCapField = 'alternateContact';
    } else if (strpos($cap_field,'primary') !== FALSE) {
        $realCapField = 'primaryContact';
    }
    if (empty($cap_field) || empty($realCapField) || !isset($capData[$realCapField])) return array();
    $capDataItem = $capData[$realCapField];
    $output = array();
    $sub_field = 'na';
    if (strpos($cap_field,'Label') !== FALSE) {
        $sub_field = 'label';
    } else if (strpos($cap_field,'Name') !== FALSE) {
        $sub_field = 'name';
    } else if (strpos($cap_field,'Title') !== FALSE) {
        $sub_field = 'title';
    } else if (strpos($cap_field,'Phone') !== FALSE) {
        $sub_field = 'phone';
    } else if (strpos($cap_field,'Email') !== FALSE) {
        $sub_field = 'email';
    }
    if (!empty($capDataItem) && is_array($capDataItem)) {
        if ($sub_field === 'phone') {
            if (!empty($capDataItem['phoneNumbers']) && is_array($capDataItem['phoneNumbers'])) {
                foreach ($capDataItem['phoneNumbers'] as $capPhone) {
                    $output[] = array('value'=>$capPhone);
//                    $output[] = array('value'=>'<div class="cap-contact-phone">'.$capPhone.'</div>',
//                        'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()));
                }
            }
        } else {
            if (!empty($capDataItem[$sub_field])) {
                $output[] = array('value'=>$capDataItem[$sub_field]);
//                $output[] = array('value'=>'<div class="cap-contact-'.$sub_field.'">'.$capDataItem[$sub_field].'</div>',
//                    'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()));
            }
        }
    }
    if (!empty($output)) {
        $output = array(LANGUAGE_NONE=>$output);
    }
    return $output;
}

/*
 * This function returns the link to the user's CAP public profile
 * Sometimes late at night I wonder if I've wasted my life...
 */
function _ses_cap_lite_get_cap_link($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem)) {
        $output = array();
    } else {
        $link = 'https://profiles.stanford.edu/'.$capDataItem;
        $output = array(LANGUAGE_NONE=>array(array('value'=>
            '<a href="'.$link.'" target="_blank">'.$link.'</a>',
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/*
 * This is a generic function that returns a CAP field stored as a simple string
 */
function _ses_cap_lite_get_string($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem)) {
        $output = array();
    } else {
        //$output = array(LANGUAGE_NONE=>array(array('value'=>'<div class="'.$cap_field.'-div">'.
        //$capDataItem.'</div>',
        $output = array(LANGUAGE_NONE=>array(array('value'=>$capDataItem, 
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/*
 * This function returns the CAP keywords field - though it is defined in the schema as
 * an array, it only seems to contain one element.
 */
function _ses_cap_lite_get_keywords($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem) || empty($capDataItem[0])) {
        $output = array();
    } else {
        $output = array(LANGUAGE_NONE=>array(array('value'=>'<div class="'.$cap_field.'-div">'.
            $capDataItem[0].'</div>',
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

/*
 * This field returns the CAP 'bigger' profile photo
 */
function _ses_cap_lite_get_profile_photo($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field]) || empty($capData['uid'])) return array();
    $sunet = $capData['uid'];
    $capDataItem = $capData[$cap_field];
    $file_array = array();
    if (!isset($capDataItem['bigger']['url'])) {
        $file_array = array();
    } else {
        $file_temp = file_get_contents($capDataItem['bigger']['url']);
        $file_temp = file_save_data($file_temp, 'public://profile_' . $sunet . '_bigger.jpg', FILE_EXISTS_REPLACE);
        $file_array = array(LANGUAGE_NONE=>array(array('fid'=>$file_temp->fid,
                                               'filename'=>$file_temp->filename,
                                               'filemime'=>$file_temp->filemime,
                                               'uri'=>$file_temp->uri, 'uid'=>1,
                                               'status'=>1, 'display'=>1)));
        image_path_flush($file_temp->uri);
    }
    return $file_array;
}

function _ses_cap_lite_get_publications_list($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem) || !is_array($capDataItem)) {
        $output = array();
    } else {
        $output = '<div class="'.$cap_field.'-div"><ul class="'.$cap_field.'-ul">';
        foreach($capDataItem as $lineitem) {
            $output .= '<li class="'.$cap_field.'-li">'.$lineitem['capCitation'].'</li>';
        }
        $output .= '</ul></div>';
        $output = array(LANGUAGE_NONE=>array(array('value'=>$output,
            'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
    }
    return $output;
}

function _ses_cap_lite_get_courses_list($capData = array(),$cap_field) {
    if (empty($cap_field) || !isset($capData[$cap_field])) return array();
    $capDataItem = $capData[$cap_field];
    if (empty($capDataItem) || !is_array($capDataItem)) {
        $output = array();
    } else {
        $output = '';
        foreach($capDataItem as $lineitem) {
            $output .= '<div class="cap_course_year">'.$lineitem['academicYear'];
            if (!empty($lineitem['courses']) && is_array($lineitem['courses'])) {
                $output .= '<ul class="'.$cap_field.'-ul">';
                foreach ($lineitem['courses'] as $course) {
                    $output .= '<li class="'.$cap_field.'-li">'.$course['label']['html'].'</li>';
                }
                $output .= '</ul>';
            }
            if (!empty($lineitem['independentStudyCourses']) && is_array($lineitem['independentStudyCourses'])) {
                $output .= '<div class="cap_course_course_label">Independent Study Courses</div>';
                $output .= '<ul class="'.$cap_field.'-ul">';
                foreach ($lineitem['independentStudyCourses'] as $course) {
                    $output .= '<li class="'.$cap_field.'-li">'.$course['label']['html'].'&nbsp'.
                               $course['label']['text'].'</li>';
                }
                $output .= '</ul>';
            }
            $output .= '</div>';
        }
        if (empty($output)) {
            $output = array();
        } else {
            $output = array(LANGUAGE_NONE=>array(array('value'=>$output,
                      'format'=>variable_get('ses_cap_lite_filter_format',filter_fallback_format()))));
        }
    }
    return $output;
}
