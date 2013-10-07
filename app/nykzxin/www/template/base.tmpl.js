<div data-role="page" id="<%= pageid %>" class="page">
  <div data-role="header">
  <% if (hasPannel) { %>
    <a href="#mypanel" data-role="button" data-icon="bars" data-iconpos="notext" data-iconshadow="false" data-inline="true">Bars</a>  <% } %>
  <h1><%= header %></h1></div>
  <div data-role="content"><%= content %></div>
  <div data-role="footer" data-position="fixed"><%= footer%></div>
  <div data-role="panel" id="mypanel" data-theme="a">
    <!-- 进入板面 -->
    <ul data-role="listview" data-theme="d" data-filter="true" data-filter-reveal="true" data-filter-placeholder="进入板面">
      <li><a href="#" onclick='gotoBoard("NetHack")'>NetHack</a></li>
    </ul>
      <!-- panel content goes here -->
  </div><!-- /panel -->
</div>
