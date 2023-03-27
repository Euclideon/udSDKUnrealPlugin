
# Unlimited Detail for Unreal guide.

> Euclideon would like to thank community members [zengweicheng666](https://github.com/zengweicheng666) and [EuleeStar](https://github.com/EuleeStar) for their Unreal Engine 4 integration which is available on [GitHub](https://github.com/zengweicheng666/UdSDKProject). The Unreal Engine 5 plugin is built upon the foundation they built for UE4.


# Table of Contents

1. [Quick start summary](#example)
2. [Indepth guide](#example2)
    - [UDCloud API Key](#example2)
    - [Installing the Unreal Plugin](#example2)
    - [Install the Unreal Plugin – Github Releases](#example2)
    - [Indepth guide ](#example2)
    - [Indepth guide ](#example2)
    - [Indepth guide ](#example2)
    - [Indepth guide ](#example2)
    - [Indepth guide ](#example2)
    


# Quick start summary

1. Ensure you have a valid UDCloud API key. A valid key can be acquired for free from:
    1. https://udcloud.com/
1.	Ensure you have Unreal 5.1+ installed: 
    1.	https://docs.unrealengine.com/5.1/en-US/installing-unreal-engine/ 
1.	Ensure you have downloaded and installed the Unlimited Detail Unreal 5 plugin:
    1.	https://my.github.url.to.the.release.of.the.plugin.com 
    1.	https://docs.unrealengine.com/5.0/en-US/working-with-plugins-in-unreal-engine/  
1.	Ensure your API key is entered into the plugins Project Settings 
    1. ![image](./mdcontent/projectSettingsAPIKey.png "Project Settings")
1.	Restart Unreal (UDS will not start rendering until your API key is logged in, restarting will force that).
1.	Add the Unlimited Detail component to either a scene actor or an existing class:
    1. ![image](./mdcontent/addComponent.png "Add component")
    1. ![image](./mdcontent/addUDSComponent.png "Component name")
1.	Assign a valid UDS URL to the component:
    1. ![image](./mdcontent/url_empty.png "Empty URL")
    1. ![image](./mdcontent/url_valid.png "Example URL")
1.	A test asset can be accessed with the following URL: https://azmodels.vault.euclideon.com/Japan/0_1_0.uds
1.	Set the component scale to something large and immediately obvious:
    1. ![image](./mdcontent/scale_large.png "Large scale")
1.	Drag the actor into the Scene to begin rendering UDS!
    1. ![image](./mdcontent/unrealuds.png "UDS in Unreal")

# Indepth guide 

## UDCloud API Key
In order to use the Unlimited Detail Unreal plugin, you require a udCloud API key.

1. Navigate to https://udcloud.com/
1. Click the sing in/sign up button in the top right:
    ![image](./mdcontent/udcloud_Welcome.png "UDS in Unreal")

1. Clicking the Sign In / Sign up button yields the following window:

    ![image](./mdcontent/udcloud_login.png "UDS in Unreal")

1. Create an account with any of the listed authentication methods.
1. After reading and accepting the EULA, Login. You will be greeted with the following window: 

    ![image](./mdcontent/udcloud_loggedin.png "UDS in Unreal")

1. Click your display name in the top right, to expand the following drop down:

    ![image](./mdcontent/udcloud_displayNameClicked.png "UDS in Unreal")

1. Click "API Keys" to load the following page: 

    ![image](./mdcontent/udcloud_APIKeys.png "UDS in Unreal")

1. Click on the top right button: 'Create API Key'

    ![image](./mdcontent/udcloud_createAPIKey.png "UDS in Unreal")
 
1. Enter a display name for the key, and (optionally) specify the number of days this key will be valid for.
1. Click **Create an API key**. It will be listed on the page like so: 

    ![image](./mdcontent/udcloud_ApiKeyList.png "UDS in Unreal")

1. Under the “Options” column, click the “...” button on far right.
    ![image](./mdcontent/udcloud_ApiKeyList.png "UDS in Unreal")

 

This will allow you to either delete the key, or copy the API key to your clipboard. 
Click Copy API Key to Clipboard

Once copied, you will see the following confirmation:

    ![image](./mdcontent/apikey_confirmation.png "UDS in Unreal")
 
The following is the result of pasting the copied clipboard API key:

`eyJhcGlrZXlpZCI6IjAxMjM0NTY3OC0xMTExLTIyMjItMzMzMy00NDQ0NTU1NTY2NjYiLCJ1c2VyaWQiOiIwMTIzNDU2NzgtMTExMS0yMjIyLTMzMzMtNDQ0NDU1NTU2NjY2IiwidmVyc2lvbiI6IjEifQ==`

This API key can be used to authenticate your account to a variety of Euclideon software. Please ensure it is kept secure at all time, as anyone with access to this exact string can use any features your account is authorized to use. We recommend (if possible) ensuring your keys have a valid expiration date suited to your purposes.

Keep this key handy, as it is what we will paste into the Unreal Plugin.
 
## Installing the Unreal Plugin

The Unlimited Detail for Unreal plugin can be acquired either directly from the Unreal Marketplace OR from our GitHub releases page, both approaches are equivalent and are detailed below:

## Install the Unreal Plugin – Unreal Marketplace

1. Ensure the Epic Games Launcher is installed. 
    - Navigate to: https://store.epicgames.com/en-US/download
2. Once Installed and launched, you will be met with the following UI:  
[SCREENSHOT]
3. Navigate to the Epic Marketplace by clicking the “Marketplace” tab:
[SCREENSHOT]
4. Locate the “Search Products” dialogue and search for “Unlimited Detail”:
[SCREENSHOT]
5. Install the plugin
[SCREENSHOT]
6. Open your project
[SCREENSHOT]
7. Navigate to plugins
[SCREENSHOT]
8. Enable the plugin
[SCREENSHOT]
9. Restart your Project

## Install the Unreal Plugin – Github Releases

Nagivate to https://my.github.url.to.the.release.of.the.plugin.com
Click “Releases” 
[SCREENSHOT HERE]
Click “download” 
[SCREENSHOT HERE]
Once downloaded, take the following files:
[SCREENSHOT HERE]
And place them within your projects “Plugins” directory.
[SCREENSHOT HERE]
Pay close attention to the plugins folder structure:
[SCREENSHOT HERE]
Once pasted into your project folder, launch your Unreal Project.

#Configuring your API key
Copy your API key from your UD Cloud account, and load your unreal project.
Your API key should look something like this: 
eyJhcGlrZXlpZCI6Ijk1ZTkxMTRjLTg4YjctNGJiNS1iM2I3LWIxZjVjZTNjZjJkNCIsInVzZXJpZCI6IjQ0MWIzNzcxLTE5NTMtNDQyOC05ODgxLTBiOTgzMjM5YjRhNiIsInZlcnNpb24iOiIxIn0=
Once loaded, navigate into your Plugins Settings dialogue:
[SCREENSHOT HERE]
Scroll down the very bottom until you see “Unlimited Detail” on the left:
[SCREENSHOT HERE]
Clicking “Unlimited detail” loads the following window:
[SCREENSHOT HERE]
Paste your API key into the API Key Dialogue:
[SCREENSHOT HERE]
Click “Login!”
[SCREENSHOT HERE]
You only ever need to click “Login” on the very first time you install the plugin (you may also restart if you prefer). The Unlimited Detail will now utilize your API key whenever you launch this project.
If your API key is ever invalid, it will display an error in the project settings like such:
[SCREENSHOT HERE]
Close the project settings window. 
Your Unlimited Detail plugin is now fully installed and configured and you can begin working with our large point cloud .UDS format.

Rendering Unlimited Detail Pointcloud within Unreal Engine
Inorder to render Point Cloud assets within Unreal Engine, you must add an Unlimited Detail component to an actor you create. 
For demonstrations sake, we will do this via blueprints (additionally a C++ example will follow below):

Create a new blueprint actor within your project
[SCREENSHOT HERE]
Open the Blueprint Actor
[SCREENSHOT HERE]
In the Top left, click Add Component
[SCREENSHOT HERE]
And Search for “Unlimited Detail”
[SCREENSHOT HERE]
Add the Unlimited Detail Component to the actor.
Once added, you should immediately see a default sample visual load in both the Blueprint inspector Viewport, and in the scene, should you place this actor into the world.
[SCREENSHOT HERE]
Ensure you have a valid URL to a .UDS asset. This can either be stored online, like so: [SCREENSHOT HERE]
… or locally, like so:
[SCREENSHOT HERE]
Locate the URL dialogue box of the Unlimited Detail component:
 
[SCREENSHOT HERE]
And paste either of the following 2 URLs into this dialogue:
[SCREENSHOT HERE OF PASTING URL OF CLOUD BASED EXAMPLE ASSET]
Result of URL 1
[SCREENSHOT HERE OF PASTING URL OF LOCALLY LOADED EXAMPLE ASSETX]
Result of URL 2 (LOCAL EXAMPLE)

Local URLs respect absolute or relative filepaths.
As the Unlimited Detail is a simple actor component, many of these components can be composited together into a single actor, like so:
[SCREENSHOT HERE]
Close the Blueprint Editor, and drag the actor from your content browser into your scene:
[SCREENSHOT HERE]
The UDS is now visualizable in the Unreal Editor.
Should you require, you can also add these components to scene actors much like any other unreal actor component: 
[SCREENSHOT HERE]
Additionally, the above can also be achieved via C++ by creating a base c++ actor, and adding the following code:
[SCREENSHOT HERE]
[COPY PASTEABLE PLAINTEXT CODE HERE]

Additional examples:
When combining the power of Unlimited Detail with the unreal engine, we’re able to make up for shortfalls inherent to pointcloud, for example:
[SHADOW CASTING VIA PROXY HULL]
[PHYSICS COLLISION WITH UDS ASSETS VIA PROXY HULL]
[ANIMATING EXAMPLE OF UDS ASSETS]

Blueprint API examples:
[LOGING IN]
[LOGING OUT]

[GETTING UDS URLS? FROM UR ACCOUNT?]
[GETTING UDS META DATA?]

[SETTING COMPONENT URL]

[FORCE COMPONENT RELOAD]

[DESTROY COMPONENT]

[DYNAMICALLY SPAWN ACTOR/COMPONNET]

