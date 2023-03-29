#ifndef udScene_h__
#define udScene_h__

//! @file udScene.h
//! The udScene and udSceneNode objects provide an interface for storing and syncronising geolocated projects between udSDK sessions
//! @note The GeoJSON provided by this implementation is not fully compliant with RFC7946
//! @warning Antimeridian Cutting (Section 3.1.9) and handling the poles (Secion 5.3) are not fully working in this implementation
//! @warning This module does not currently expose the functionality to sync with the server. This will be added in a future release.

#include "udDLLExport.h"
#include "udError.h"
#include "udMathTypes.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

  struct udContext;

  //!
  //! @struct udScene
  //! Stores the internal state of the project
  //!
  struct udScene;

  //!
  //! @struct udSceneNodeInternal
  //! Stores the internal state of a udSceneNode
  //!
  struct udSceneNodeInternal;

  //!
  //! These are the geometry types for nodes
  //!
  enum udSceneGeometryType
  {
    udPGT_None, //!< There is no geometry associated with this node

    udPGT_Point, //!< pCoordinates is a single 3D position
    udPGT_MultiPoint, //!< Array of udPGT_Point, pCoordinates is an array of 3D positions
    udPGT_LineString, //!< pCoordinates is an array of 3D positions forming an open line
    udPGT_MultiLineString, //!< Array of udPGT_LineString; pCoordinates is NULL and children will be present
    udPGT_Polygon, //!< pCoordinates will be a closed linear ring (the outside), there MAY be children that are interior as pChildren udPGT_MultiLineString items, these should be counted as islands of the external ring.
    udPGT_MultiPolygon, //!< pCoordinates is null, children will be udPGT_Polygon (which still may have internal islands)
    udPGT_GeometryCollection, //!< Array of geometries; pCoordinates is NULL and children may be present of any type

    udPGT_Count, //!< Total number of geometry types. Used internally but can be used as an iterator max when displaying different type modes.

    udPGT_Internal, //!< Used internally when calculating other types. Do not use.
  };

  //!
  //! This represents the type of data stored in the node.
  //! @note The `itemtypeStr` in the udSceneNode is a string version. This enum serves to simplify the reading of standard types. The the string in brackets at the end of the comment is the string.
  //!
  enum udSceneNodeType
  {
    udPNT_Custom, //!< Need to check the itemtypeStr string manually

    udPNT_PointCloud, //!< A Euclideon Unlimited Detail Point Cloud file ("UDS")
    udPNT_PointOfInterest, //!< A point, line or region describing a location of interest ("POI")
    udPNT_Folder, //!< A folder of other nodes ("Folder")
    udPNT_Media, //!< An Image, Movie, Audio file or other media object ("Media")
    udPNT_Viewpoint, //!< An Camera Location & Orientation ("Camera")
    udPNT_VisualisationSettings, //!< Visualisation settings (itensity, map height etc) ("VizSet")
    udPNT_I3S, //!< An Indexed 3d Scene Layer (I3S) or Scene Layer Package (SLPK) dataset ("I3S")
    udPNT_Water, //!< A region describing the location of a body of water ("Water")
    udPNT_ViewShed, //!< A point describing where to generate a view shed from ("ViewMap")
    udPNT_Polygon, //!< A polygon model, usually an OBJ or FBX ("Polygon")
    udPNT_QueryFilter, //!< A query filter to be applied to all PointCloud types in the scene ("QFilter")
    udPNT_Places, //!< A collection of places that can be grouped together at a distance ("Places")
    udPNT_HeightMeasurement, //!< A height measurement object ("MHeight")
    udPNT_GTFS, //!< A General Transit Feed Specification object ("GTFS")
    udPNT_LassoNode, //!< A Lasso Selection Folder ("LNode")
    udPNT_QueryGroup, //!< A Group of Query node being represented as one node ("QGroup")
    udPNT_Count //!< Total number of node types. Used internally but can be used as an iterator max when displaying different type modes.
  };

  enum udSceneUserFlag
  {
    udSceneUserFlag_HasHands = 1, //!< User has hands
    udSceneUserFlag_HasAnchorAndScale = 2, //!< User has a world anchor point. This usually indicates they are using a VR Cave or Table.
  };

  //!
  //! @struct udScenePosition
  //! This represents a position in 3D
  //! @note contains x,y,z and heading, pitch (generally of a camera) in the project CRS
  //! 
  struct udScenePosition
  {
    double x; //!< The x coordiante of the item
    double y; //!< The y coordiante of the item
    double z; //!< The z coordiante of the item

    double heading; //!< The heading of the item
    double pitch; //!< The pitch of the item
    double roll; //!< The roll of the item
  };

  //!
  //! @struct udSceneNodeSelected
  //! This represents the selected scene node of a user
  //!
  struct udSceneNodeSelected
  {
    char id[37]; //!< The uuid of the selected node
  };

  //!
  //! @struct udSceneMessage
  //! This represents the message sent to other users (or to us) in a scene during collaboration
  //! 
  struct udSceneMessage
  {
    const char *pMessageType; //!< The type of the message
    const char *pMessagePayload; //!< The payload of the message

    const char *pTargetSessionID; //!< The session ID of the message
    const char *pReceivedFromSessionID; //!< The session Id where it's been received
  };

  //!
  //! @struct udSceneUser
  //! This represents the user info (used for collaboration)
  //! 
  struct udSceneUser
  {
    const char *pDisplayName; //!< The name to display for this user
    const char *pUserID; //!< The uuid of the user
    const char *pSceneSessionID; //!< The current scene/session ID this user is log in
    double lastUpdated; //!< The time its position was been last updated [server time]

    enum udSceneUserFlag userFlags; //!< Flag to know if user is using XR

    uint32_t selectedNodesCount; //!< The number of node selected
    struct udSceneNodeSelected *selectedNodesList; //!< The selected nodes

    struct udScenePosition headCamera; //!< The position of the head or camera for the active viewer
    struct udScenePosition handRight; //!< The position of the right hand for the active viewer if udSceneUserFlag_HasHands is set in userFlags
    struct udScenePosition handLeft; //!< The position of the left hand for the active viewer if udSceneUserFlag_HasHands is set in userFlags
    struct udMathDouble4 worldAnchorPoint; //!< The position of the world anchor point if udSceneUserFlag_HasAnchorAndScale is set; xyz are a position in world space and w is the scale factor.
  };

  //!
  //! @struct udSceneNode
  //! Stores the state of a node.
  //! @warning This struct is exposed to avoid having a huge API of accessor functions but it should be treated as read only with the exception of `pUserData`. Making changes to the internal data will cause issues syncronising data
  //!
  struct udSceneNode
  {
    // Node header data
    int isVisible; //!< Non-zero if the node is visible and should be drawn in the scene
    char UUID[37]; //!< Unique identifier for this node "id"
    double lastUpdate; //!< The last time this node was updated in UTC

    enum udSceneNodeType itemtype; //!< The type of this node, see udSceneNodeType for more information
    char itemtypeStr[8]; //!< The string representing the type of node. If its a known type during node creation `itemtype` will be set to something other than udPNT_Custom

    const char *pName; //!< Human readable name of the item
    const char *pURI; //!< The address or filename that the resource can be found.

    uint32_t hasBoundingBox; //!< Set to not 0 if this nodes boundingBox item is filled out
    double boundingBox[6]; //!< The bounds of this model, ordered as [West, South, Floor, East, North, Ceiling]

    // Geometry Info
    enum udSceneGeometryType geomtype; //!< Indicates what geometry can be found in this model. See the udSceneGeometryType documentation for more information.
    int geomCount; //!< How many geometry items can be found on this model
    double *pCoordinates; //!< The positions of the geometry of this node (NULL this this node doesn't have points). The format is [X0,Y0,Z0,...Xn,Yn,Zn]

    // Parent node
    struct udSceneNode *pParent; //!< This is the parent item of the current node (NULL if root node)

    // Next nodes
    struct udSceneNode *pNextSibling; //!< This is the next item in the scene (NULL if no further siblings)
    struct udSceneNode *pFirstChild; //!< Some types ("folder", "collection", "polygon"...) have children nodes, NULL if there are no children.

    // Node Data
    void (*pUserDataCleanup)(struct udSceneNode *pNode, void *pUserData); //!< When a project node is deleted, this function is called first
    void *pUserData; //!< This is application specific user data. The application should traverse the tree to release these before releasing the udScene
    struct udSceneNodeInternal *pInternalData; //!< Internal udSDK specific state for this node
  };

  //!
  //! This represents where the scene was loaded from/saved to most recently and where future calls to udScene_Save will go
  //!
  enum udSceneLoadSource
  {
    udSceneLoadSource_Memory, //!< The scene source exists in memory; udScene_CreateInMemory, udScene_LoadFromMemory or udScene_SaveToMemory
    udSceneLoadSource_Server, //!< The scene source exists from the server; udScene_CreateInServer, udScene_LoadFromServer or udScene_SaveToServer
    udSceneLoadSource_URI, //!< The scene source exists from a file or URL; udScene_CreateInFile, udScene_LoadFromFile or udScene_SaveToFile

    udSceneLoadSource_Count //!< Total number of source types. Used internally but can be used as an iterator max when displaying different source types.
  };

  //!
  //! @struct udSceneUpdateInfo
  //! This represents the update info given/received to/by udScene_Update
  //! @warning Memory is Freed on next call of udScene_Updte()
  //! 
  struct udSceneUpdateInfo
  {
    uint32_t forceSync; //!< If this is non-zero the sync to the server will happen immediately and the update call will block
    double lastUpdatedTime; //!< The timestamp that the scene was last updated (local time)

    enum udSceneUserFlag userFlags; //!< User flags to define the device and other properties

    uint32_t selectedNodesCount; //!< The length of pSelectedNodesList
    struct udSceneNodeSelected *pSelectedNodesList; //!< The list of selected nodes

    struct udScenePosition headCamera; //!< The position of the head or camera for the active viewer
    struct udScenePosition handRight; //!< The position of the right hand for the active viewer if udSceneUserFlag_HasHands is set in userFlags
    struct udScenePosition handLeft; //!< The position of the left hand for the active viewer if udSceneUserFlag_HasHands is set in userFlags
    struct udMathDouble4 worldAnchorPoint; //!< The position of the world anchor point if udSceneUserFlag_HasAnchorAndScale is set; xyz are a position in world space and w is the scale factor.

    struct udSceneUser *pUserList; //!< The list of other users in this scene
    uint32_t usersCount; //!< The number of users in the scene (and length of pUserList)

    struct udSceneMessage *pReceivedMessages; //!< The list of messages
    uint32_t receivedMessagesCount; //!< The length of pReceivedMessages
  };

  //!
  //! Create an empty, local only, instance of `udScene`.
  //!
  //! @param pContext The pointer to the udContext of the session to use to creae in memory
  //! @param ppScene The pointer pointer of the udScene. This will allocate an instance of udScene into `ppScene`.
  //! @param pName The name of the scene, this will be the name of the root item.
  //! @return A udError value based on the result of the project creation.
  //!
  UDSDKDLL_API enum udError udScene_CreateInMemory(struct udContext *pContext, struct udScene **ppScene, const char *pName);

  //!
  //! Create an empty, local only, instance of `udScene`.
  //!
  //! @param pContext The pointer to the udContext of the session to use to create ina file
  //! @param ppScene The pointer pointer of the udScene. This will allocate an instance of udScene into `ppScene`.
  //! @param pName The name of the project, this will be the name of the root item.
  //! @param pFilename The path to create the project at.
  //! @return A udError value based on the result of the project creation.
  //!
  UDSDKDLL_API enum udError udScene_CreateInFile(struct udContext *pContext, struct udScene **ppScene, const char *pName, const char *pFilename);

  //!
  //! Create an empty project in the server and the local instance of `udScene`.
  //!
  //! @param pContext The pointer to the udContext of the session to use to load the project, read/write permissions will be based on the current session.
  //! @param ppScene The pointer pointer of the udScene. This will allocate an instance of udScene into `ppScene`.
  //! @param pName The name of the project, this will be the name of the root item.
  //! @param pGroupID The UUID of the group this project will belong to
  //! @return A udError value based on the result of the project creation.
  //!
  UDSDKDLL_API enum udError udScene_CreateInServer(struct udContext *pContext, struct udScene **ppScene, const char *pName, const char *pGroupID);

  //!
  //! Create a local only instance of `udScene` filled in with the contents of a GeoJSON string.
  //!
  //! @param pContext The pointer to the udContext of the session to use to load from memory
  //! @param ppScene The pointer pointer of the udScene. This will allocate an instance of udScene into `ppScene`.
  //! @param pGeoJSON The GeoJSON string of the project, this will be unpacked into udSceneNode items.
  //! @return A udError value based on the result of the project creation.
  //!
  UDSDKDLL_API enum udError udScene_LoadFromMemory(struct udContext *pContext, struct udScene **ppScene, const char *pGeoJSON);

  //!
  //! Create a local only instance of `udScene` filled in with the contents of the specified project file.
  //!
  //! @param pContext The pointer to the udContext of the session to use to load from a file
  //! @param ppScene The pointer pointer of the udScene. This will allocate an instance of udScene into `ppScene`.
  //! @param pFilename The project file URL.
  //! @return A udError value based on the result of the project creation.
  //!
  UDSDKDLL_API enum udError udScene_LoadFromFile(struct udContext *pContext, struct udScene **ppScene, const char *pFilename);

  //!
  //! Create a local instance of `udScene` filled in from the server.
  //!
  //! @param pContext The pointer to the udContext of the session to use to load the project, read/write permissions will be based on the current session.
  //! @param ppScene The pointer pointer of the udScene. This will allocate an instance of udScene into `ppScene`.
  //! @param pSceneUUID The UUID for the project that is being requested
  //! @param pGroupID The ID for the workspace/project for udCloud projects (null for udServer projects)
  //! @return A udError value based on the result of the project creation.
  //!
  UDSDKDLL_API enum udError udScene_LoadFromServer(struct udContext *pContext, struct udScene **ppScene, const char *pSceneUUID, const char *pGroupID);

  //!
  //! Update a local instance of `udScene` by merging elements from another `udScene`.
  //!
  //! @param pContext The pointer to the udContext of the session to use to load the project, read/write permissions will be based on the current session.
  //! @param pScene The pointer of the udScene to update.
  //! @param pSceneToMerge The pointer of the udScene to merge its elements into previous udScene
  //! @return A udError value based on the result of the merge.
  //! @note pSceneToMerge will be empty after this operation.
  //!
  UDSDKDLL_API enum udError udScene_MergeScenes(struct udContext *pContext, struct udScene *pScene, struct udScene *pSceneToMerge);

  //!
  //! Destroy the instance of the project.
  //!
  //! @param ppScene The pointer pointer of the udScene. This will deallocate the instance of the project as well as destroying all nodes.
  //! @return A udError value based on the result of the project destruction.
  //! @warning The pUserData item should be already released from every node before calling this.
  //!
  UDSDKDLL_API enum udError udScene_Release(struct udScene **ppScene);

  //!
  //! Export a project to where it was loaded from (server or file).
  //!
  //! @param pScene The pointer to a valid udScene that will saved. This will fail immediately for projects loaded from memory
  //! @return A udError value based on the result of the project save
  //!
  UDSDKDLL_API enum udError udScene_Save(struct udScene *pScene);

  //!
  //! Update a project to where it was loaded from (server or file).
  //!
  //! @param pScene The pointer to a valid udScene that will saved. This will fail immediately for projects loaded from memory
  //! @param pUpdateInfo The data that would be updated
  //! @return A udError value based on the result of the project save
  //!
  UDSDKDLL_API enum udError udScene_Update(struct udScene *pScene, struct udSceneUpdateInfo *pUpdateInfo);

  //!
  //! Export a project to a GeoJSON string in memory.
  //!
  //! @param pContext The pointer to the udContext of the session to use to save the project to memory.
  //! @param pScene The pointer to a valid udScene that will be exported as GeoJSON.
  //! @param ppMemory The pointer pointer to a string that will contain the exported GeoJSON.
  //! @return A udError value based on the result of the project export.
  //! @warning The memory is stored in the udScene and subsequent calls will destroy older versions, the buffer is released when the project is released.
  //!
  UDSDKDLL_API enum udError udScene_SaveToMemory(struct udContext *pContext, struct udScene *pScene, const char **ppMemory);

  //!
  //! Create an project in the server from an existing udScene
  //!
  //! @param pContext The pointer to the udContext of the session to use to upload the project, write permissions will be based on the current session.
  //! @param pScene The udScene to upload.
  //! @param pGroupID The UUID of the group this project will belong to
  //! @return A udError value based on the result of the project creation.
  //! @note pScene will point to the newly created project if this is successful
  //!
  UDSDKDLL_API enum udError udScene_SaveToServer(struct udContext *pContext, struct udScene *pScene, const char *pGroupID);

  //!
  //! Create an project in the server from an existing udScene
  //!
  //! @param pContext The pointer to the udContext of the session to use to save the project to file.
  //! @param pScene The udScene to save out.
  //! @param pPath The new path to save the project to
  //! @return A udError value based on the result of the project creation.
  //! @note pScene will point to the newly created file if this is successful
  //!
  UDSDKDLL_API enum udError udScene_SaveToFile(struct udContext *pContext, struct udScene *pScene, const char *pPath);

  //!
  //! Get the project root node.
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param ppRootNode The pointer pointer to a node that will contain the node. The node ownership still belongs to the pScene.
  //! @return A udError value based on the result of getting the root node.
  //! @note This node will always have itemtype of type udPNT_Folder and cannot (and will not) have sibling nodes.
  //! @note The name of the project is the name of the returned root node.
  //!
  UDSDKDLL_API enum udError udScene_GetProjectRoot(struct udScene *pScene, struct udSceneNode **ppRootNode);

  //!
  //! Get the project UUID (for server projects).
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param ppSceneUUID The pointer pointer to memory that will contain the project UUID. The ownership still belongs to the pScene.
  //! @return A udError value based on the result of getting the root node.
  //! @note Returns udE_NotFound for non-server projects
  //!
  UDSDKDLL_API enum udError udScene_GetProjectUUID(struct udScene *pScene, const char **ppSceneUUID);

  //!
  //! Get the state of unsaved local changes
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @return udE_Success if there a unsaved changes, udE_NotFound when no local changes exist and other udError values for errors
  //!
  UDSDKDLL_API enum udError udScene_HasUnsavedChanges(struct udScene *pScene);

  //!
  //! Gets the current source of a given udScene
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param pSource The pointer to memory to write the source of pScene to
  //! @return udE_Success if wrote source, error otherwise
  //!
  UDSDKDLL_API enum udError udScene_GetLoadSource(struct udScene *pScene, enum udSceneLoadSource *pSource);

  //!
  //! Create a node in a project
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param ppNode A pointer pointer to the node that will be created. This can be NULL if getting the node back isn't required.
  //! @param pParent The parent node of the item.
  //! @param pType The string representing the type of the item. This can be a defined string provided by udScene_GetTypeName or a custom type. Cannot be NULL.
  //! @param pName A human readable name for the item. If this item is NULL it will attempt to generate a name from the pURI or the pType strings.
  //! @param pURI The URL, filename or other URI containing where to find this item. These should be absolute paths where applicable (preferably HTTPS) to ensure maximum interop with other packages.
  //! @param pUserData A pointer to data provided by the application to store in the `pUserData` item in the udSceneNode.
  //! @return A udError value based on the result of the node being created in the project.
  //!
  UDSDKDLL_API enum udError udSceneNode_Create(struct udScene *pScene, struct udSceneNode **ppNode, struct udSceneNode *pParent, const char *pType, const char *pName, const char *pURI, void *pUserData);

  //!
  //! Move a node to reorder within the current parent or move to a different parent.
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param pCurrentParent The current parent of pNode.
  //! @param pNewParent The intended new parent of pNode.
  //! @param pNode The node to move.
  //! @param pInsertBeforeChild The node that will be after pNode after the move. Set as NULL to be the last child of pNewParent.
  //! @return A udError value based on the result of the move.
  //!
  UDSDKDLL_API enum udError udSceneNode_MoveChild(struct udScene *pScene, struct udSceneNode *pCurrentParent, struct udSceneNode *pNewParent, struct udSceneNode *pNode, struct udSceneNode *pInsertBeforeChild);

  //!
  //! Remove a node from the project.
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param pParentNode The parent of the node to be removed.
  //! @param pNode The node to remove from the project.
  //! @return A udError value based on the result of removing the node.
  //! @warning Ensure that the pUserData point of pNode has been released before calling this.
  //!
  UDSDKDLL_API enum udError udSceneNode_RemoveChild(struct udScene *pScene, struct udSceneNode *pParentNode, struct udSceneNode *pNode);

  //!
  //! Set the human readable name of a node.
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param pNode The node to change the name of.
  //! @param pNodeName The new name of the node. This is duplicated internally.
  //! @return A udError value based on the result of setting the name.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetName(struct udScene *pScene, struct udSceneNode *pNode, const char *pNodeName);

  //!
  //! Set the visibility of the node
  //!
  //! @param pNode The node to change the visibility.
  //! @param visibility The new visibility of the node (non-zero for visible).
  //! @return A udError value based on the result of updating the visibility.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetVisibility(struct udSceneNode *pNode, int visibility);

  //!
  //! Set the URI of a node.
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param pNode The node to change the name of.
  //! @param pNodeURI The new URI of the node. This is duplicated internally.
  //! @return A udError value based on the result of setting the URI.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetURI(struct udScene *pScene, struct udSceneNode *pNode, const char *pNodeURI);

  //!
  //! Set a bounding box for the node.
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param pNode The node to change the bounding box of.
  //! @param boundingBox An array of doubles representing the bounds of the node, ordered as [West, South, Floor, East, North, Ceiling]
  //! @return A udError value based on the result of setting the bounding box.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetBoundingBox(struct udScene *pScene, struct udSceneNode *pNode, double boundingBox[6]);

  //!
  //! Set the new geometry of a node.
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param pNode The node to change the geometry of.
  //! @param nodeType The new type of the geometry
  //! @param geometryCount How many coordinates are being added
  //! @param pCoordinates A pointer to the new coordinates (this is an array that should be 3x the length of geometryCount). Ordered in [ longitude0, latitude0, altitude0, ..., longitudeN, latitudeN, altitudeN ] order.
  //! @return A udError value based on the result of setting the geometry.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetGeometry(struct udScene *pScene, struct udSceneNode *pNode, enum udSceneGeometryType nodeType, int geometryCount, double *pCoordinates);

  //!
  //! Get a metadata item of a node as an integer.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param pInt The pointer to the memory to write the metadata to
  //! @param defaultValue The value to write to pInt if the metadata item isn't in the udSceneNode or if it isn't of an integer type
  //! @return A udError value based on the result of reading the metadata into pInt.
  //!
  UDSDKDLL_API enum udError udSceneNode_GetMetadataInt(struct udSceneNode *pNode, const char *pMetadataKey, int32_t *pInt, int32_t defaultValue);

  //!
  //! Set a metadata item of a node from an integer.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param iValue The integer value to write to the metadata key
  //! @return A udError value based on the result of writing the metadata into the node.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetMetadataInt(struct udSceneNode *pNode, const char *pMetadataKey, int32_t iValue);

  //!
  //! Get a metadata item of a node as an unsigned integer.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param pInt The pointer to the memory to write the metadata to
  //! @param defaultValue The value to write to pInt if the metadata item isn't in the udSceneNode or if it isn't of an integer type
  //! @return A udError value based on the result of reading the metadata into pInt.
  //!
  UDSDKDLL_API enum udError udSceneNode_GetMetadataUint(struct udSceneNode *pNode, const char *pMetadataKey, uint32_t *pInt, uint32_t defaultValue);

  //!
  //! Set a metadata item of a node from an unsigned integer.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param iValue The unsigned integer value to write to the metadata key
  //! @return A udError value based on the result of writing the metadata into the node.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetMetadataUint(struct udSceneNode *pNode, const char *pMetadataKey, uint32_t iValue);

  //!
  //! Get a metadata item of a node as a 64 bit integer.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param pInt64 The pointer to the memory to write the metadata to
  //! @param defaultValue The value to write to pInt64 if the metadata item isn't in the udSceneNode or if it isn't of an integer type
  //! @return A udError value based on the result of reading the metadata into pInt64.
  //!
  UDSDKDLL_API enum udError udSceneNode_GetMetadataInt64(struct udSceneNode *pNode, const char *pMetadataKey, int64_t *pInt64, int64_t defaultValue);

  //!
  //! Set a metadata item of a node from a 64 bit integer.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param i64Value The integer value to write to the metadata key
  //! @return A udError value based on the result of writing the metadata into the node.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetMetadataInt64(struct udSceneNode *pNode, const char *pMetadataKey, int64_t i64Value);

  //!
  //! Get a metadata item of a node as a double.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param pDouble The pointer to the memory to write the metadata to
  //! @param defaultValue The value to write to pDouble if the metadata item isn't in the udSceneNode or if it isn't of an integer or floating point type
  //! @return A udError value based on the result of reading the metadata into pDouble.
  //!
  UDSDKDLL_API enum udError udSceneNode_GetMetadataDouble(struct udSceneNode *pNode, const char *pMetadataKey, double *pDouble, double defaultValue);

  //!
  //! Set a metadata item of a node from a double.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param doubleValue The double value to write to the metadata key
  //! @return A udError value based on the result of writing the metadata into the node.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetMetadataDouble(struct udSceneNode *pNode, const char *pMetadataKey, double doubleValue);

  //!
  //! Get a metadata item of a node as a boolean.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param pBool The pointer to the memory to write the metadata to (0=false, !0=true)
  //! @param defaultValue The value to write to pBool if the metadata item isn't in the udSceneNode or if it isn't of a boolean type
  //! @return A udError value based on the result of reading the metadata into pBool.
  //!
  UDSDKDLL_API enum udError udSceneNode_GetMetadataBool(struct udSceneNode *pNode, const char *pMetadataKey, uint32_t *pBool, uint32_t defaultValue);

  //!
  //! Set a metadata item of a node from a boolean.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param boolValue The boolean value to write to the metadata key (0=false, !0=true)
  //! @return A udError value based on the result of writing the metadata into the node.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetMetadataBool(struct udSceneNode *pNode, const char *pMetadataKey, uint32_t boolValue);

  //!
  //! Get a metadata item of a node as a string.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param ppString The pointer pointer to the memory of the string. This is owned by the udSceneNode and should be copied if required to be stored for a long period.
  //! @param pDefaultValue The value to write to ppString if the metadata item isn't in the udSceneNode or if it isn't of a string type
  //! @return A udError value based on the result of reading the metadata into ppString.
  //!
  UDSDKDLL_API enum udError udSceneNode_GetMetadataString(struct udSceneNode *pNode, const char *pMetadataKey, const char **ppString, const char *pDefaultValue);

  //!
  //! Set a metadata item of a node from a string.
  //!
  //! @param pNode The node to get the metadata from.
  //! @param pMetadataKey The name of the metadata key.
  //! @param pString The string to write to the metadata key. This is duplicated internally.
  //! @return A udError value based on the result of writing the metadata into the node.
  //!
  UDSDKDLL_API enum udError udSceneNode_SetMetadataString(struct udSceneNode *pNode, const char *pMetadataKey, const char *pString);

  //!
  //! Get the standard type string name for an itemtype
  //!
  //! @param itemtype The udSceneNodeType value
  //! @return A string containing the standard name for the item in the udSceneNodeType enum. This is internal and should not be freed.
  //!
  UDSDKDLL_API const char* udScene_GetTypeName(enum udSceneNodeType itemtype); // Might return NULL

  //!
  //! Deletes a project from the server
  //!
  //! @param pContext The udContext to use to communicate with the server
  //! @param pSceneUUID The UUID of the project to delete
  //! @param pGroupID The ID for the workspace/project for udCloud projects (null for udServer projects)
  //! @return A udError result (udE_Success if it was deleted, other status codes depending on result)
  //!
  UDSDKDLL_API enum udError udScene_DeleteServerProject(struct udContext *pContext, const char *pSceneUUID, const char *pGroupID);

  //!
  //! Sets the share status of a project on the server
  //!
  //! @param pContext The udContext to use to communicate with the server
  //! @param pSceneUUID The UUID of the project to update the share status of
  //! @param isSharableToAnyoneWithTheLink Not 0 if the project should be able to be loaded by anyone with the link, 0 otherwise
  //! @param pGroupID The ID for the workspace/project for udCloud projects (null for udServer projects)
  //! @return A udError result (udE_Success if it was updated, other status codes depending on result)
  //!
  UDSDKDLL_API enum udError udScene_SetLinkShareStatus(struct udContext *pContext, const char *pSceneUUID, uint32_t isSharableToAnyoneWithTheLink, const char *pGroupID);

  //!
  //! Get the session ID (for server projects).
  //!
  //! @param pScene The pointer to a valid udScene.
  //! @param ppSessionID The pointer pointer to memory that will contain the session ID. The ownership still belongs to the pScene.
  //! @return A udError value based on the result of getting the root node.
  //! @note Returns udE_NotFound for non-server projects
  //!
  UDSDKDLL_API enum udError udScene_GetSessionID(struct udScene *pScene, const char **ppSessionID);

  //!
  //! Queues a message to be sent on next project update (for server projects).
  //!
  //! @param pScene The pointer to the active udScene.
  //! @param pTargetSessionID The session ID of the user who will receive this message. Passing in 'nullptr' to this parameter indicates all users (except the sender) will receive this message.
  //! @param pMessageType pMessageType User defined message type.
  //! @param pMessagePayload User defined payload.
  //! @return A udError result (udE_Success if the message was successfully queued, other status codes depending on result)
  //!
  UDSDKDLL_API enum udError udScene_QueueMessage(struct udScene *pScene, const char *pTargetSessionID, const char *pMessageType, const char *pMessagePayload);

  //!
  //! Saves a project thumbnail in base64 format to the server (udCloud only).
  //!
  //! @param pScene The pointer to a valid udScene to save the thumbnail to.
  //! @param pImageBase64 The base64 encoded thumbnail.
  //! @return A udError value based on the result of the save.
  //!
  UDSDKDLL_API enum udError udScene_SaveThumbnail(struct udScene* pScene, const char* pImageBase64);

#ifdef __cplusplus
}
#endif

#endif // udScene_h__
