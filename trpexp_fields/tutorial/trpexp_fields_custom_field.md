
The best way to add new content to a Tripal Content Type(s) is to create a custom field. This example shows you how to add a stock summary to your Tripal Organism pages by 

 - creating a custom field to pull out the summary numbers, 
 - a default widget to block editing of the summary info and 
 - a custom field formatter to display the summary as a table.

## Choosing Ontology Terms.
In Tripal 3 all fields are associated to an ontolgy term which is reflected in the name of the field itself. As such, before we can begin writing the code for our custom field, we first need to determine what term to use to describe it. It is recommended that you find a term from a public ontology that intuitively described your field. In the case of our example, we were unable to find an appropriate term in published ontologies and as such resorted to adding the term "germplasm_summary" to our "local" controlled vocabulary. This should be avoided if at all possible.

> **Convention**: Fields are named based on their ontolgoy term: [cv name]__[cvterm name]. Thus our field will be named "local__germplasm_summary", since our ontology term is "germplasm_summary" from the "local" vocabulary.

> **Keep in mind**: It is difficult to change the ontology term choosen above once you begin developing your field since it is named after the term. We recommend you ensure you have choosen the best term possible before moving on to the next step.

## Create default classes for your custom field.
The first step to creating your custom field is to extend the base TripalField or ChadoField classes. If your custom field is independant of Chado then you can extend TripalField directly. However, if either of the following is true then you should extend the ChadoField classes and mark your module as dependant on tripal_chado:

 - you use a value stored in chado (e.g. our example uses organism.organism_id to pull out the summary information)
 - your data is stored in a custom chado table or materialized view (e.g. our summary is stored in the organism_stock_counts materialized view)

Based on the above criteria, we need to create a ChadoField. 

We recommend that you don't include any of your custom functionality in these classes at this point since it complicates the debugging process. 
