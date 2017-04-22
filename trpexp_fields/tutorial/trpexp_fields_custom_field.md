
The best way to add new content to a Tripal Content Type(s) is to create a custom field. This example shows you how to add a stock summary to your Tripal Organism pages by 

 - creating a custom field to pull out the summary numbers, 
 - a default widget to block editing of the summary info and 
 - a custom field formatter to display the summary as a table.

## 1. Choosing Ontology Terms.
In Tripal 3 all fields are associated to an ontolgy term which is reflected in the name of the field itself. As such, before we can begin writing the code for our custom field, we first need to determine what term to use to describe it. 

> **Convention**: Fields are named based on their ontolgoy term: [cv name]__[cvterm name]. Thus our field will be named "local__germplasm_summary", since our ontology term is "germplasm_summary" from the "local" vocabulary.

It is recommended that you find a term from a public ontology that intuitively described your field. In the case of our example, we were unable to find an appropriate term in published ontologies and as such resorted to adding the term "germplasm_summary" to our "local" controlled vocabulary. This should be avoided if at all possible.

> **Keep in mind**: It is difficult to change the ontology term choosen above once you begin developing your field since it is named after the term. We recommend you ensure you have choosen the best term possible before moving on to the next step.

## 2. Create default classes for your custom field.
The first step to creating your custom field is to extend the base TripalField or ChadoField classes. If your custom field is independant of Chado then you can extend TripalField directly. However, if either of the following is true then you should extend the ChadoField classes and mark your module as dependant on tripal_chado:

 - you use a value stored in chado (e.g. our example uses organism.organism_id to pull out the summary information)
 - your data is stored in a custom chado table or materialized view (e.g. our summary is stored in the organism_stock_counts materialized view)

Based on the above criteria, we need to create a ChadoField since both our data and the field we want to filter our data are stored in Chado. The process is very much the same for creating a TripalField as it is for creating a ChadoField with the main difference being which classes you extend in this step. As such we will show you how to create both TripalFields and ChadoFields in this step even though only ChadoFields apply directly to our example. Make sure to **only follow A _OR_ B below** depending on the requirements for your custom field.

### A. Create ChadoField classes.
We recommend that you don't include any of your custom functionality in these classes at this point since it complicates the debugging process. As such we've created some empty extension classes distributed with this module that you can simply copy and rename during this step. To create a ChadoField, you need THREE classes:

- `CUSTOM_CHADOFIELD.inc`: a field class to handle loading of your data
- `CUSTOM_CHADOWIDGET.inc`: a widget class to handle form elements on a Tripal Content add/edit form
- `CUSTOM_CHADOFORMATTER.inc`: a formatter class to handle the display of your data on a Tripal Content page

These classes need to be created in a specific directory in your custom module in order to facillitate smart loading by Tripal. Simply copy the above classes from this module to `[your custom module directory]/includes/TripalFields/[your field name]/` making sure that each class is named as follows: field, `[field name].inc`; widget, `[field name]_widget.inc`; formatter, `[field name]_formatter.inc`.

```
cd [your drupal root]/sites/all/modules
cp tripal_example/trpexp_fields/includes/TripalFields/CUSTOM_CHADOFIELD/CUSTOM_CHADOFIELD.inc [your custom module directory]/includes/TripalFields/[your field name]/[your field name].inc
cp tripal_example/trpexp_fields/includes/TripalFields/CUSTOM_CHADOFIELD/CUSTOM_CHADOWIDGET.inc [your custom module directory]/includes/TripalFields/[your field name]/[your field name]_widget.inc
cp tripal_example/trpexp_fields/includes/TripalFields/CUSTOM_CHADOFIELD/CUSTOM_CHADOFORMATTER.inc [your custom module directory]/includes/TripalFields/[your field name]/[your field name]_formatter.inc
```

Next, edit each class and replace `CUSTOM-FIELDNAME` with the name your field. Thus `class CUSTOM-FIELDNAME extends ChadoField {` becomes `class local__germplasm_summary extends ChadoField {` in our example. Notice that the widget and formatter classes have a suffix added to the name so `class CUSTOM-FIELDNAME_widget extends ChadoFieldWidget {` becomes `class local__germplasm_summary_widget extends ChadoFieldWidget {`.

### B. Create TripalField classes.
We recommend that you don't include any of your custom functionality in these classes at this point since it complicates the debugging process.  As such we've created some empty extension classes distributed with this module that you can simply copy and rename during this step. To create a TripalField, you need THREE classes:

- `CUSTOM_TRIPALFIELD.inc`: a field class to handle loading of your data
- `CUSTOM_TRIPALWIDGET.inc`: a widget class to handle form elements on a Tripal Content add/edit form
- `CUSTOM_TRIPALFORMATTER.inc`: a formatter class to handle the display of your data on a Tripal Content page

These classes need to be created in a specific directory in your custom module in order to facillitate smart loading by Tripal. Simply copy the above classes from this module to `[your custom module directory]/includes/TripalFields/[your field name]/` making sure that each class is named as follows: field, `[field name].inc`; widget, `[field name]_widget.inc`; formatter, `[field name]_formatter.inc`.

```
cd [your drupal root]/sites/all/modules
cp tripal_example/trpexp_fields/includes/TripalFields/CUSTOM_TRIPALFIELD/CUSTOM_TRIPALFIELD.inc [your custom module directory]/includes/TripalFields/[your field name]/[your field name].inc
cp tripal_example/trpexp_fields/includes/TripalFields/CUSTOM_TRIPALFIELD/CUSTOM_TRIPALWIDGET.inc [your custom module directory]/includes/TripalFields/[your field name]/[your field name]_widget.inc
cp tripal_example/trpexp_fields/includes/TripalFields/CUSTOM_TRIPALFIELD/CUSTOM_TRIPALFORMATTER.inc [your custom module directory]/includes/TripalFields/[your field name]/[your field name]_formatter.inc
```

Next, edit each class and replace `CUSTOM-FIELDNAME` with the name your field. Thus `class CUSTOM-FIELDNAME extends TripalField {` becomes `class local__germplasm_summary extends TripalField {` in our example. Notice that the widget and formatter classes have a suffix added to the name so `class CUSTOM-FIELDNAME_widget extends TripalFieldWidget {` becomes `class local__germplasm_summary_widget extends TripalFieldWidget {`.

### Documentation

It is also advisable to add documententation to the top of these classes specifying 
- All: the purpose of your field
- Field: what data your custom field accesses
- Field: any assumptions you're making about the data
- Field: if this field is dependant on a materialized view then state that and the materialized view needs to be populated
- Widget: whether you're allowing editing through the interface
- Widget: where the data is being stored
- Formatter: what you're expending the value of this field to look like
- Formatter: what kind of display your providing (e.g. table, pie chart, etc.)
- Formatter: what settings you expose to allow configuration of the display

For examples of good docuentation, see local__germplasm_summary.inc, local__germplasm_summary_widget.inc, local__germplasm_summary_formatter.inc.

## 3. Register your custom field with Tripal/Drupal.

## 4. Programmatically add your field to an existing Tripal Content Type (optional).

## 5. Add custom functionality to your field, widget and formatter classes.
