/**
 * @file sound_ids.cpp
 * @brief Temporary holder for sound IDs.
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 *
 * Copyright (c) 2001-2009, Linden Research, Inc.
 *
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 *
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
 *
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 *
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "sound_ids.h"

#include "lluuid.h"

LLUUID const SND_NULL = LLUUID::null;
LLUUID const SND_RIDE		   ("00000000-0000-0000-0000-000000000100");
LLUUID const SND_SHOT		   ("00000000-0000-0000-0000-000000000101");
LLUUID const SND_MORTAR		   ("00000000-0000-0000-0000-000000000102");
LLUUID const SND_HIT		   ("00000000-0000-0000-0000-000000000103");
LLUUID const SND_EXPLOSION	   ("00000000-0000-0000-0000-000000000104");
LLUUID const SND_BOING		   ("00000000-0000-0000-0000-000000000105");
LLUUID const SND_OBJECT_CREATE ("9f1bc096-3592-411e-9b0b-c447a9ff054c");

//
//  Different bird sounds for different states
//

LLUUID const SND_CHIRP		("00000000-0000-0000-0000-000000000106");			//  Flying random chirp
LLUUID const SND_CHIRP2		("828a9526-175b-455d-8af0-0e3c0fb602b2");			//  Spooked by user
LLUUID const SND_CHIRP3		("f99772d6-1ce6-4a39-a28b-06d26c94c9e3");			//  Spooked by object
LLUUID const SND_CHIRP4		("54472ca4-7fc9-42cb-b7d5-99ad5b12bd50");			//  Chasing other bird
LLUUID const SND_CHIRP5		("2929964f-fac5-40d7-9179-2864a8fa9ace");			//  Hopping random chirp
LLUUID const SND_CHIRPDEAD  ("9abff1d3-863a-4e04-bd83-3834fd7fcff4");			//  Hit by grenade - dead!


LLUUID const SND_MUNCH		("00000000-0000-0000-0000-000000000107");
LLUUID const SND_PUNCH		("00000000-0000-0000-0000-000000000108");
LLUUID const SND_SPLASH		("00000000-0000-0000-0000-000000000109");
LLUUID const SND_CLICK		("00000000-0000-0000-0000-000000000110");
LLUUID const SND_WHISTLE	("ab858f9a-1f44-4d39-9b33-351543d03ccb");
LLUUID const SND_TYPING     ("5e191c7b-8996-9ced-a177-b2ac32bfea06");

LLUUID const SND_ARROW_SHOT	("00000000-0000-0000-0000-000000000111");
LLUUID const SND_ARROW_THUD	("00000000-0000-0000-0000-000000000112");
LLUUID const SND_LASER_SHOT	("00000000-0000-0000-0000-000000000113");
LLUUID const SND_JET_THRUST	("67f5e4f0-0534-4d97-bc01-f297648d20e0");

LLUUID const SND_SILENCE	("00000000-0000-0000-0000-000000000114");
LLUUID const SND_BUBBLES	("00000000-0000-0000-0000-000000000115");
LLUUID const SND_WELCOME	("00000000-0000-0000-0000-000000000116");
LLUUID const SND_SQUISH	    ("00000000-0000-0000-0000-000000000117");
LLUUID const SND_SUBPOD	    ("00000000-0000-0000-0000-000000000118");
LLUUID const SND_FOOTSTEPS  ("00000000-0000-0000-0000-000000000119");
LLUUID const SND_STEP_LEFT  ("00000000-0000-0000-0000-000000000124");
LLUUID const SND_STEP_RIGHT ("00000000-0000-0000-0000-000000000125");

LLUUID const SND_BALL_COLLISION	("00000000-0000-0000-0000-000000000120");

LLUUID const SND_OOOH_SCARE_ME ("00000000-0000-0000-0000-000000000121");
LLUUID const SND_PAYBACK_TIME  ("00000000-0000-0000-0000-000000000122");
LLUUID const SND_READY_FOR_BATTLE ("00000000-0000-0000-0000-000000000123");

LLUUID const SND_FLESH_FLESH      ("dce5fdd4-afe4-4ea1-822f-dd52cac46b08");
LLUUID const SND_FLESH_PLASTIC    ("51011582-fbca-4580-ae9e-1a5593f094ec");
LLUUID const SND_FLESH_RUBBER     ("68d62208-e257-4d0c-bbe2-20c9ea9760bb");
LLUUID const SND_GLASS_FLESH      ("75872e8c-bc39-451b-9b0b-042d7ba36cba");
LLUUID const SND_GLASS_GLASS      ("6a45ba0b-5775-4ea8-8513-26008a17f873");
LLUUID const SND_GLASS_PLASTIC    ("992a6d1b-8c77-40e0-9495-4098ce539694");
LLUUID const SND_GLASS_RUBBER     ("2de4da5a-faf8-46be-bac6-c4d74f1e5767");
LLUUID const SND_GLASS_WOOD       ("6e3fb0f7-6d9c-42ca-b86b-1122ff562d7d");
LLUUID const SND_METAL_FLESH      ("14209133-4961-4acc-9649-53fc38ee1667");
LLUUID const SND_METAL_GLASS      ("bc4a4348-cfcc-4e5e-908e-8a52a8915fe6");
LLUUID const SND_METAL_METAL      ("9e5c1297-6eed-40c0-825a-d9bcd86e3193");
LLUUID const SND_METAL_PLASTIC    ("e534761c-1894-4b61-b20c-658a6fb68157");
LLUUID const SND_METAL_RUBBER     ("8761f73f-6cf9-4186-8aaa-0948ed002db1");
LLUUID const SND_METAL_WOOD       ("874a26fd-142f-4173-8c5b-890cd846c74d");
LLUUID const SND_PLASTIC_PLASTIC  ("0e24a717-b97e-4b77-9c94-b59a5a88b2da");
LLUUID const SND_RUBBER_PLASTIC   ("75cf3ade-9a5b-4c4d-bb35-f9799bda7fb2");
LLUUID const SND_RUBBER_RUBBER    ("153c8bf7-fb89-4d89-b263-47e58b1b4774");
LLUUID const SND_STONE_FLESH      ("55c3e0ce-275a-46fa-82ff-e0465f5e8703");
LLUUID const SND_STONE_GLASS      ("24babf58-7156-4841-9a3f-761bdbb8e237");
LLUUID const SND_STONE_METAL      ("aca261d8-e145-4610-9e20-9eff990f2c12");
LLUUID const SND_STONE_PLASTIC    ("0642fba6-5dcf-4d62-8e7b-94dbb529d117");
LLUUID const SND_STONE_RUBBER     ("25a863e8-dc42-4e8a-a357-e76422ace9b5");
LLUUID const SND_STONE_STONE      ("9538f37c-456e-4047-81be-6435045608d4");
LLUUID const SND_STONE_WOOD       ("8c0f84c3-9afd-4396-b5f5-9bca2c911c20");
LLUUID const SND_WOOD_FLESH       ("be582e5d-b123-41a2-a150-454c39e961c8");
LLUUID const SND_WOOD_PLASTIC     ("c70141d4-ba06-41ea-bcbc-35ea81cb8335");
LLUUID const SND_WOOD_RUBBER      ("7d1826f4-24c4-4aac-8c2e-eff45df37783");
LLUUID const SND_WOOD_WOOD        ("063c97d3-033a-4e9b-98d8-05c8074922cb");


LLUUID const SND_SLIDE_FLESH_FLESH      ("614eec22-f73d-4fdc-8691-a37dc5c58333");
LLUUID const SND_SLIDE_FLESH_PLASTIC    (SND_NULL);
LLUUID const SND_SLIDE_FLESH_RUBBER     (SND_NULL);
LLUUID const SND_SLIDE_FLESH_FABRIC     ("3678b9b9-2a0c-42b5-9c83-80b64ad6e898");
LLUUID const SND_SLIDE_FLESH_GRAVEL     ("02eaa42a-ce1a-4b6b-9c38-cd7ad0e8f4a6");
LLUUID const SND_SLIDE_FLESH_GRAVEL_02  ("e7d3b501-79f8-4419-b842-ab6843e0f840");
LLUUID const SND_SLIDE_FLESH_GRAVEL_03  ("4c3e8b52-6244-4e44-85a6-f4ab994418ed");
LLUUID const SND_SLIDE_GLASS_GRAVEL     ("ca491e77-5c47-4ea1-8021-b3ebbf636cab");
LLUUID const SND_SLIDE_GLASS_GRAVEL_02  ("30794d49-91ce-48e3-a527-c06f67bd6cbe");
LLUUID const SND_SLIDE_GLASS_GRAVEL_03  ("04c78e54-fd8d-46b6-8ab9-7678b5d6e5cb");
LLUUID const SND_SLIDE_GLASS_FLESH      (SND_NULL);
LLUUID const SND_SLIDE_GLASS_GLASS      (SND_NULL);
LLUUID const SND_SLIDE_GLASS_PLASTIC    (SND_NULL);
LLUUID const SND_SLIDE_GLASS_RUBBER     (SND_NULL);
LLUUID const SND_SLIDE_GLASS_WOOD       (SND_NULL);
LLUUID const SND_SLIDE_METAL_FABRIC     ("18b66e81-2958-42d4-a373-7a5054919adc");
LLUUID const SND_SLIDE_METAL_FLESH      ("dde65837-633c-4841-af2f-62ec471bf61e");
LLUUID const SND_SLIDE_METAL_FLESH_02   ("f3cc2cbe-1a1a-4db7-a8d2-e9c8f8fa1f4f");
LLUUID const SND_SLIDE_METAL_GLASS      ("4188be39-7b1f-4495-bf2b-83ddd82eea05");
LLUUID const SND_SLIDE_METAL_GLASS_02   ("336faa2b-9d96-4e14-93ad-b63b60074379");
LLUUID const SND_SLIDE_METAL_GLASS_03   ("34d912aa-cf73-4462-b7d0-dcba2c66caba");
LLUUID const SND_SLIDE_METAL_GLASS_04   ("97ffc063-e872-4469-8e95-1450ac6bad2b");
LLUUID const SND_SLIDE_METAL_GRAVEL     ("2bbff37d-009a-4cfc-9a0d-817652c08fbe");
LLUUID const SND_SLIDE_METAL_GRAVEL_02  ("a906a228-783b-49e7-9f0a-e20a41d0e39f");
LLUUID const SND_SLIDE_METAL_METAL      ("09461277-c691-45de-b2c5-89dfd3712f79");
LLUUID const SND_SLIDE_METAL_METAL_02   ("e00a5d97-8fdc-46c1-bd53-7e312727466c");
LLUUID const SND_SLIDE_METAL_METAL_03   ("8ebfa780-c440-4b52-ab65-5edf3bc15bf1");
LLUUID const SND_SLIDE_METAL_METAL_04   ("d6d03cb2-5b16-4e31-b7d4-2a81d2a0909b");
LLUUID const SND_SLIDE_METAL_METAL_05   ("3a46f447-916e-47de-a1e5-95d1af46bd0f");
LLUUID const SND_SLIDE_METAL_METAL_06   ("cd423231-e70d-4fd2-ad26-f1c6cf5f0610");
LLUUID const SND_SLIDE_METAL_PLASTIC    (SND_NULL);
LLUUID const SND_SLIDE_METAL_RUBBER     ("12d97bc0-3c15-4744-b6bd-77d1316eb4f0");
LLUUID const SND_SLIDE_METAL_WOOD       ("4afb6926-a73f-4cb7-85d5-0f9a40107434");
LLUUID const SND_SLIDE_METAL_WOOD_02    ("349970bf-187d-4bcb-b2cf-e7bb6581590f");
LLUUID const SND_SLIDE_METAL_WOOD_03    ("64bf6e87-73d4-4cb4-84f7-55cecfd97cd3");
LLUUID const SND_SLIDE_METAL_WOOD_04    ("0dc670a9-dbe8-41bc-b8ee-4d96d99219d5");
LLUUID const SND_SLIDE_METAL_WOOD_05    ("6e3cc57b-c9aa-4829-86a1-8e82aeaccb47");
LLUUID const SND_SLIDE_METAL_WOOD_06    ("c1237f4c-8c88-4da1-bfbc-2af26a8d9e5a");
LLUUID const SND_SLIDE_METAL_WOOD_07    ("0e1ec243-063b-4dcb-a903-52b8dffed3d2");
LLUUID const SND_SLIDE_METAL_WOOD_08    ("66736d0f-533d-4007-a8ee-0f27c2034126");
LLUUID const SND_SLIDE_PLASTIC_GRAVEL   ("35092c21-5c48-4b4d-a818-3cf240af2348");
LLUUID const SND_SLIDE_PLASTIC_GRAVEL_02("c37f5776-0020-47e8-89a0-c74cc6f5742d");
LLUUID const SND_SLIDE_PLASTIC_GRAVEL_03("d2fc8db6-2e66-464a-8ccb-f99b61ee4987");
LLUUID const SND_SLIDE_PLASTIC_GRAVEL_04("93cbdb10-6e82-4c0b-a547-7b3b79ac25f6");
LLUUID const SND_SLIDE_PLASTIC_GRAVEL_05("2f6d0542-fcd1-4264-a17b-f57bf5ebf402");
LLUUID const SND_SLIDE_PLASTIC_GRAVEL_06("5b8887d4-3be2-45a0-b25d-85af3b1e6392");
LLUUID const SND_SLIDE_PLASTIC_PLASTIC     (SND_NULL);
LLUUID const SND_SLIDE_PLASTIC_PLASTIC_02  (SND_NULL);
LLUUID const SND_SLIDE_PLASTIC_PLASTIC_03  (SND_NULL);
LLUUID const SND_SLIDE_PLASTIC_FABRIC   ("7294d9ad-3e41-4373-992c-a9f21d5d66ad");
LLUUID const SND_SLIDE_PLASTIC_FABRIC_02("58608ce1-f524-472f-b447-bbe6ce4a46e0");
LLUUID const SND_SLIDE_PLASTIC_FABRIC_03("06ae285e-0b34-4ea6-84ab-9c6c31b414fc");
LLUUID const SND_SLIDE_PLASTIC_FABRIC_04("211613db-0461-49bd-9554-5c14ad8b31f6");
LLUUID const SND_SLIDE_RUBBER_PLASTIC      ("a98ffa5a-e48e-4f9d-9242-b9a3210ad84a");
LLUUID const SND_SLIDE_RUBBER_PLASTIC_02   ("d4136c40-eeaa-49c6-a982-8e5a16f5d93a");
LLUUID const SND_SLIDE_RUBBER_PLASTIC_03   ("29ec0fb2-0b23-47b2-835b-c83cc7cf9fb0");
LLUUID const SND_SLIDE_RUBBER_RUBBER    (SND_NULL);
LLUUID const SND_SLIDE_STONE_FLESH      (SND_NULL);
LLUUID const SND_SLIDE_STONE_GLASS      (SND_NULL);
LLUUID const SND_SLIDE_STONE_METAL      (SND_NULL);
LLUUID const SND_SLIDE_STONE_PLASTIC    ("afd0bcc3-d41a-4572-9e7f-08a29eeb0b8a");
LLUUID const SND_SLIDE_STONE_PLASTIC_02 ("881b720a-96cf-4128-bb98-5d87e03e93c7");
LLUUID const SND_SLIDE_STONE_PLASTIC_03 ("293dac42-658a-4c5a-a7a2-6d4c5e5658b0");
LLUUID const SND_SLIDE_STONE_RUBBER     ("0724b946-6a3f-4eeb-bb50-0a3b33120974");
LLUUID const SND_SLIDE_STONE_RUBBER_02  ("ada93d00-76e2-4bf1-9ad9-493727630717");
LLUUID const SND_SLIDE_STONE_STONE      ("ade766dc-2e75-4699-9b41-7c8e53d2b3f2");
LLUUID const SND_SLIDE_STONE_STONE_02   ("66698375-6594-47b0-8046-c3973de1291d");
LLUUID const SND_SLIDE_STONE_WOOD       ("174ef324-ed50-4f65-9479-b4da580aeb3c");
LLUUID const SND_SLIDE_STONE_WOOD_02    ("33d517fd-ff11-4d01-a7b5-0e3abf818dcf");
LLUUID const SND_SLIDE_STONE_WOOD_03    ("1bac4b63-e6fd-4659-9761-991284cf4582");
LLUUID const SND_SLIDE_STONE_WOOD_04    ("a7d28564-6821-4c01-a378-cde98fba7ba9");
LLUUID const SND_SLIDE_WOOD_FABRIC      ("22c58e74-22cd-4960-9ab7-5bf08ab824e5");
LLUUID const SND_SLIDE_WOOD_FABRIC_02   ("0b0ed22e-4a0f-4617-a4cf-20d0f2b78ccc");
LLUUID const SND_SLIDE_WOOD_FABRIC_03   ("42b80abb-9823-4b74-a210-326ccf23636a");
LLUUID const SND_SLIDE_WOOD_FABRIC_04   ("8538298a-1e6b-4b69-a9ee-5e01e4a02b35");
LLUUID const SND_SLIDE_WOOD_FLESH       ("84b026f3-a11c-4366-aa7c-07edcd89b2bb");
LLUUID const SND_SLIDE_WOOD_FLESH_02    ("2644191f-4848-47ba-8ba7-bddc0bfcb3da");
LLUUID const SND_SLIDE_WOOD_FLESH_03    ("edb978e4-9be9-456f-b2fc-e8502bfe25be");
LLUUID const SND_SLIDE_WOOD_FLESH_04    ("bf2b972e-f42a-46d7-b53e-5fca38f5bc61");
LLUUID const SND_SLIDE_WOOD_GRAVEL      ("d063bb4d-0eff-4403-a6cc-c6c6c073e624");
LLUUID const SND_SLIDE_WOOD_GRAVEL_02   ("511eb679-6d93-47fa-9141-c3ef9261c919");
LLUUID const SND_SLIDE_WOOD_GRAVEL_03   ("4ed1fd43-4707-4e5c-b7b7-21ec4e72c1ac");
LLUUID const SND_SLIDE_WOOD_GRAVEL_04   ("99ea89b3-aa76-4b87-99c8-670365c6d8c3");
LLUUID const SND_SLIDE_WOOD_PLASTIC     ("505ca3c4-94a0-4e28-8fc1-ea72a428396b");
LLUUID const SND_SLIDE_WOOD_PLASTIC_02  ("fc404011-df71-4ed0-8f22-b72bdd18f63c");
LLUUID const SND_SLIDE_WOOD_PLASTIC_03  ("67dbe225-26df-4efa-8c8b-f1ef669fec45");
LLUUID const SND_SLIDE_WOOD_RUBBER      (SND_NULL);
LLUUID const SND_SLIDE_WOOD_WOOD        ("3079d569-b3e8-4df4-9e09-f0d4611213ef");
LLUUID const SND_SLIDE_WOOD_WOOD_02     ("276b093d-dbcb-4279-a89e-a54b0b416af6");
LLUUID const SND_SLIDE_WOOD_WOOD_03     ("c3f3ca5e-2768-4081-847f-247139310fdb");
LLUUID const SND_SLIDE_WOOD_WOOD_04     ("f08d44b8-ff87-4a98-9561-c72f1f2fec81");
LLUUID const SND_SLIDE_WOOD_WOOD_05     ("2d8a58cf-f139-4238-8503-27d334d05c85");
LLUUID const SND_SLIDE_WOOD_WOOD_06     ("e157ebbd-b12d-4225-aa7c-d47b026a7687");
LLUUID const SND_SLIDE_WOOD_WOOD_07     ("35e17956-e7b4-478c-b274-e37db8a166b2");
LLUUID const SND_SLIDE_WOOD_WOOD_08     ("e606fc65-0643-4964-9979-ff964fa6a62c");


LLUUID const SND_ROLL_FLESH_FLESH      (SND_NULL);
LLUUID const SND_ROLL_FLESH_PLASTIC    ("89a0be4c-848d-4a6e-8886-298f56c2cff4");
LLUUID const SND_ROLL_FLESH_PLASTIC_02 ("beb06343-1aa1-4af2-b320-5d2ec31c53b1");
LLUUID const SND_ROLL_FLESH_RUBBER     (SND_NULL);
LLUUID const SND_ROLL_GLASS_GRAVEL     ("ba795c74-7e09-4572-b495-e09886a46b86");
LLUUID const SND_ROLL_GLASS_GRAVEL_02  ("4c93c3b7-14cb-4d9b-a7df-628ad935f1f2");
LLUUID const SND_ROLL_GLASS_FLESH      (SND_NULL);
LLUUID const SND_ROLL_GLASS_GLASS      (SND_NULL);
LLUUID const SND_ROLL_GLASS_PLASTIC    (SND_NULL);
LLUUID const SND_ROLL_GLASS_RUBBER     (SND_NULL);
LLUUID const SND_ROLL_GLASS_WOOD       ("d40b1f48-a061-4f6e-b18f-4326a3dd5c29");
LLUUID const SND_ROLL_GLASS_WOOD_02    ("78cd407a-bb36-4163-ba09-20f2e6d9d44b");
LLUUID const SND_ROLL_GRAVEL_GRAVEL    ("c7354cc3-6df5-4738-8dbb-b28a6ac46a05");
LLUUID const SND_ROLL_GRAVEL_GRAVEL_02 ("01d194c4-72a6-47df-81a5-8db430faff87");
LLUUID const SND_ROLL_METAL_FABRIC     ("ce6e6564-20fd-48e4-81e2-cd3f81c00a3e");
LLUUID const SND_ROLL_METAL_FABRIC_02  ("fc4d0065-32f6-4bb0-9f3f-f4737eb27163");
LLUUID const SND_ROLL_METAL_FLESH      (SND_NULL);
LLUUID const SND_ROLL_METAL_GLASS      ("63d530bb-a41f-402b-aa1f-be6b11959809");
LLUUID const SND_ROLL_METAL_GLASS_02   ("f62642c2-6db5-4faa-8b77-939067d837c3");
LLUUID const SND_ROLL_METAL_GLASS_03   ("db5b5a15-2817-4cd7-9f0b-9ad49b5e52c8");
LLUUID const SND_ROLL_METAL_GRAVEL     ("447164e3-9646-4c1a-a16d-606892891466");
LLUUID const SND_ROLL_METAL_METAL      ("c3c22cf3-5d1f-4cc3-b4b5-708b9f65979c");
LLUUID const SND_ROLL_METAL_METAL_02   ("d8386277-a1ea-460e-b6fd-bb285c323bf1");
LLUUID const SND_ROLL_METAL_METAL_03   ("69ee1f02-f9cd-4c8b-aedd-39a2d6705680");
LLUUID const SND_ROLL_METAL_METAL_04   ("5cc6b5fd-26ce-47ad-b21d-3a7c190dd375");
LLUUID const SND_ROLL_METAL_PLASTIC    ("c6a9bbf6-df15-4713-9f84-7237fce4051e");
LLUUID const SND_ROLL_METAL_PLASTIC_01 ("0fedb59b-2dbb-4cec-b6cc-8559ec027749");
LLUUID const SND_ROLL_METAL_RUBBER     (SND_NULL);
LLUUID const SND_ROLL_METAL_WOOD       ("1d76af57-01b1-4c73-9a1d-69523bfa50ea");
LLUUID const SND_ROLL_METAL_WOOD_02    ("78aa4e71-8e7c-4b90-a561-3ebdc639f99b");
LLUUID const SND_ROLL_METAL_WOOD_03    ("777d95bf-962f-48fa-93bf-8c1806557d72");
LLUUID const SND_ROLL_METAL_WOOD_04    ("1833da76-45e2-4a8b-97da-d17413e056c9");
LLUUID const SND_ROLL_METAL_WOOD_05    ("b13e1232-3d8d-42e9-92ec-b30f9f823962");
LLUUID const SND_ROLL_PLASTIC_FABRIC   ("616a1f03-209f-4c55-b264-83a000b6ef0a");
LLUUID const SND_ROLL_PLASTIC_PLASTIC    ("873f3d82-00b2-4082-9c69-7aef3461dba1");
LLUUID const SND_ROLL_PLASTIC_PLASTIC_02 ("cc39879f-ebc8-4405-a4fc-8342f5bed31e");
LLUUID const SND_ROLL_RUBBER_PLASTIC   (SND_NULL);
LLUUID const SND_ROLL_RUBBER_RUBBER    (SND_NULL);
LLUUID const SND_ROLL_STONE_FLESH      (SND_NULL);
LLUUID const SND_ROLL_STONE_GLASS      (SND_NULL);
LLUUID const SND_ROLL_STONE_METAL      (SND_NULL);
LLUUID const SND_ROLL_STONE_PLASTIC    ("155f65a8-cae7-476e-a58b-fd362be7fd0e");
LLUUID const SND_ROLL_STONE_RUBBER     (SND_NULL);
LLUUID const SND_ROLL_STONE_STONE      ("67d56e3f-6ed5-4658-9418-14f020c38b11");
LLUUID const SND_ROLL_STONE_STONE_02   ("43d99d10-d75b-4246-accf-4ceb2c909aa7");
LLUUID const SND_ROLL_STONE_STONE_03   ("f04e83ff-eed7-4e99-8f45-eb97e4e1d3b7");
LLUUID const SND_ROLL_STONE_STONE_04   ("10fcc5ad-fa89-48d6-b774-986b580c1efc");
LLUUID const SND_ROLL_STONE_STONE_05   ("3d86f5a3-1a91-49d9-b99f-8521a7422497");
LLUUID const SND_ROLL_STONE_WOOD       ("53e46fb7-6c21-4fe1-bffe-0567475d48fa");
LLUUID const SND_ROLL_STONE_WOOD_02    ("5eba8c9a-a014-4299-87f1-315c45ec795b");
LLUUID const SND_ROLL_STONE_WOOD_03    ("ea6c05fc-6e9c-4526-8a20-bc47810bb549");
LLUUID const SND_ROLL_STONE_WOOD_04    ("64618cbf-3f42-4728-8094-e77807545efb");
LLUUID const SND_ROLL_WOOD_FLESH       ("26ee185d-6fc3-49f8-89ba-51cab04cfc42");
LLUUID const SND_ROLL_WOOD_FLESH_02    ("334faa25-1e80-4c99-b29f-4c9c2a3d079d");
LLUUID const SND_ROLL_WOOD_FLESH_03    ("2f876626-4dce-4f71-a91e-a25302edfab7");
LLUUID const SND_ROLL_WOOD_FLESH_04    ("d6877aac-07fc-4931-bcde-585f223802ad");
LLUUID const SND_ROLL_WOOD_GRAVEL      ("2a23ebb5-a4a2-4f1f-8d75-7384239354aa");
LLUUID const SND_ROLL_WOOD_GRAVEL_02   ("208bf26d-f097-450c-95c4-9d26317c613c");
LLUUID const SND_ROLL_WOOD_GRAVEL_03   ("a26ecaf4-92c6-4e32-9864-56b7c70cab8e");
LLUUID const SND_ROLL_WOOD_PLASTIC     ("71c1000a-9f16-4cc3-8ede-ec4aa3bf5723");
LLUUID const SND_ROLL_WOOD_PLASTIC_02  ("7bc20ba6-1e6d-4eea-83ad-c5cc3ae0e409");
LLUUID const SND_ROLL_WOOD_RUBBER      (SND_NULL);
LLUUID const SND_ROLL_WOOD_WOOD        ("2cc8eec4-bb4a-4ba8-b783-71526ec708e8");
LLUUID const SND_ROLL_WOOD_WOOD_02     ("0a1f8070-a11a-4b4c-b260-5ffb6acb0a5d");
LLUUID const SND_ROLL_WOOD_WOOD_03     ("160bef64-da9c-4be8-b07b-a5060b501700");
LLUUID const SND_ROLL_WOOD_WOOD_04     ("1c62ea16-cc60-48ed-829a-68b8f4cf0c1c");
LLUUID const SND_ROLL_WOOD_WOOD_05     ("be9cc8fe-b920-4bf5-8924-453088cbc03f");
LLUUID const SND_ROLL_WOOD_WOOD_06     ("a76cfe60-56b0-43b1-8f31-93e56947d78b");
LLUUID const SND_ROLL_WOOD_WOOD_07     ("0c6aa481-b5bc-4573-ae83-8e16ff27e750");
LLUUID const SND_ROLL_WOOD_WOOD_08     ("214ab2c7-871a-451b-b0db-4c5677199011");
LLUUID const SND_ROLL_WOOD_WOOD_09     ("0086e4db-3ac6-4545-b414-6f359bedd9a5");

LLUUID const SND_SLIDE_STONE_STONE_01	("2a7dcbd1-d3e6-4767-8432-8322648e7b9d");

LLUUID const SND_STONE_DIRT_01			("97727335-392c-4338-ac4b-23a7883279c2");
LLUUID const SND_STONE_DIRT_02			("cbe75eb2-3375-41d8-9e3f-2ae46b4164ed");
LLUUID const SND_STONE_DIRT_03			("31e236ee-001b-4c8e-ad6c-c2074cb64357");
LLUUID const SND_STONE_DIRT_04			("c8091652-e04b-4a11-84ba-15dba06e7a1b");

LLUUID const SND_STONE_STONE_02			("ba4ef5ac-7435-4240-b826-c24ba8fa5a78");
LLUUID const SND_STONE_STONE_04			("ea296329-0f09-4993-af1b-e6784bab1dc9");
