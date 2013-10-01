 var tip = Backbone.Model.extend({

    // Default attributes for the todo item.
    defaults: {
        title: "空帖子",
		author:"空作者",
        tip_id:1234567,
		content:"空内容",
		board:"未知板面",
    },

    // Ensure that each todo created has `title`.
    initialize: function() {
     
    },


    // Remove this Todo from *localStorage* and delete its view.
    clear: function() {
      this.destroy();
    }
  });
  
 
 
 
 
  var tipList = Backbone.Collection.extend({

    // Reference to this collection's model.
    model: tip,

    // Save all of the todo items under the `"todos"` namespace.
    localStorage: new Store("todos-backbone"),

    // Filter down the list of all todo items that are finished.
    done: function() {
      return this.filter(function(todo){ return todo.get('done'); });
    },

    // Filter down the list to only todo items that are still not finished.
    remaining: function() {
      return this.without.apply(this, this.done());
    },

    // We keep the Todos in sequential order, despite being saved by unordered
    // GUID in the database. This generates the next order number for new items.
    nextOrder: function() {
      if (!this.length) return 1;
      return this.last().get('order') + 1;
    },

    // Todos are sorted by their original insertion order.
    comparator: function(todo) {
      return todo.get('order');
    }

  });



  
  
  
  
  
  